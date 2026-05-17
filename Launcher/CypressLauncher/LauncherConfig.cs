#nullable enable
using System;
using System.IO;
using Newtonsoft.Json.Linq;

namespace CypressLauncher;

internal static class LauncherConfig
{
    public static readonly string MasterUrl = "https://api-cypress.v0e.dev";
    public static readonly string RelayNA = "na-relay.v0e.dev:25200";
    public static readonly string RelayEU = "eu-relay.v0e.dev:25200";

    static LauncherConfig()
    {
        try
        {
            string path = Path.Combine(AppContext.BaseDirectory, "launcher.config.json");
            if (!File.Exists(path)) return;

            var j = JObject.Parse(File.ReadAllText(path));
            if (j["masterUrl"] is JToken mu && mu.Type == JTokenType.String)
                MasterUrl = mu.Value<string>()!;
            if (j["relayServers"] is JObject relays)
            {
                if (relays["na"] is JToken na && na.Type == JTokenType.String)
                    RelayNA = na.Value<string>()!;
                if (relays["eu"] is JToken eu && eu.Type == JTokenType.String)
                    RelayEU = eu.Value<string>()!;
            }
        }
        catch { }
    }
}
