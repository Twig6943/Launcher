package ea

import (
	"encoding/json"
	"fmt"
	"io"
	"log"
	"net/http"
	"time"
)

const entitlementsURL = "https://gateway.ea.com/proxy/identity/pids/me/entitlements?expand=entitlement"

type entitlementEntry struct {
	EntitlementID   int64  `json:"entitlementId"`
	EntitlementTag  string `json:"entitlementTag"`
	EntitlementType string `json:"entitlementType"`
	GroupName       string `json:"groupName"`
	ProjectID       string `json:"projectId"`
	Status          string `json:"status"`
}

type entitlementListResp struct {
	Entitlements struct {
		Entitlement    []entitlementEntry `json:"entitlement"`
		EntitlementUri []string           `json:"entitlementUri"`
	} `json:"entitlements"`
}

var gameGroupNames = map[string]bool{
	"PVZGWPC":  true, // GW1
	"PVZGW2PC": true, // GW2
	"PVZGW3PC": true, // BFN
}

var httpClient = &http.Client{Timeout: 10 * time.Second}

// FetchGameEntitlements returns the first active ONLINE_ACCESS entitlementId
// for each PvZ game, keyed by groupName (PVZGWPC, PVZGW2PC, PVZGW3PC).
func FetchGameEntitlements(accessToken string) (map[string]string, error) {
	req, err := http.NewRequest("GET", entitlementsURL, nil)
	if err != nil {
		return nil, err
	}
	req.Header.Set("Authorization", "Bearer "+accessToken)

	resp, err := httpClient.Do(req)
	if err != nil {
		return nil, fmt.Errorf("entitlements request failed: %w", err)
	}
	defer resp.Body.Close()

	if resp.StatusCode != 200 {
		body, _ := io.ReadAll(io.LimitReader(resp.Body, 512))
		return nil, fmt.Errorf("entitlements returned %d: %s", resp.StatusCode, string(body))
	}

	body, err := io.ReadAll(io.LimitReader(resp.Body, 256*1024))
	if err != nil {
		return nil, fmt.Errorf("reading entitlements body: %w", err)
	}

	var listResp entitlementListResp
	if err := json.Unmarshal(body, &listResp); err != nil {
		return nil, fmt.Errorf("parsing entitlements: %w", err)
	}

	entries := listResp.Entitlements.Entitlement

	// API returned URIs instead of objects - fetch each one individually
	if len(entries) == 0 && len(listResp.Entitlements.EntitlementUri) > 0 {
		log.Printf("[ea] entitlements: got %d URIs, fetching individually", len(listResp.Entitlements.EntitlementUri))
		for _, uri := range listResp.Entitlements.EntitlementUri {
			fullURL := "https://gateway.ea.com/proxy/identity" + uri
			ereq, err := http.NewRequest("GET", fullURL, nil)
			if err != nil {
				continue
			}
			ereq.Header.Set("Authorization", "Bearer "+accessToken)
			eresp, err := httpClient.Do(ereq)
			if err != nil {
				continue
			}
			ebody, _ := io.ReadAll(io.LimitReader(eresp.Body, 4096))
			eresp.Body.Close()
			if eresp.StatusCode != 200 {
				continue
			}
			var single struct {
				Entitlement entitlementEntry `json:"entitlement"`
			}
			if json.Unmarshal(ebody, &single) == nil && single.Entitlement.GroupName != "" {
				e := single.Entitlement
				log.Printf("[ea] entitlements: uri entry group=%s type=%s status=%s id=%d", e.GroupName, e.EntitlementType, e.Status, e.EntitlementID)
				entries = append(entries, e)
			} else {
				log.Printf("[ea] entitlements: uri %s parse failed or empty groupName. body: %.256s", uri, string(ebody))
			}
		}
	}

	result := make(map[string]string)
	for _, e := range entries {
		if !gameGroupNames[e.GroupName] {
			continue
		}
		if e.EntitlementType != "ONLINE_ACCESS" {
			continue
		}
		if e.Status != "ACTIVE" {
			continue
		}
		if _, exists := result[e.GroupName]; !exists {
			result[e.GroupName] = fmt.Sprintf("%d", e.EntitlementID)
		}
	}

	if len(result) == 0 {
		log.Printf("[ea] entitlements: 0 matches from %d entries. raw body: %.512s", len(entries), string(body))
	}

	return result, nil
}
