// ea auth login screen logic

window._eaLoggedIn = false;
window._identityOwnedGames = [];
window._identityEaRelinked = false;

function updateProfileIdentitySection() {
    var loggedIn = !!window._eaLoggedIn;
    var signedOutBlock = document.getElementById('profileSignedOutBlock');
    var signedInBlock = document.getElementById('profileSignedInBlock');
    var usernameField = document.getElementById('username');
    var usernameHint = document.getElementById('usernameHint');

    if (signedOutBlock) signedOutBlock.style.display = loggedIn ? 'none' : '';
    if (signedInBlock) signedInBlock.style.display = loggedIn ? '' : 'none';

    if (usernameField) {
        if (loggedIn) {
            usernameField.readOnly = true;
            usernameField.oninput = null;
            usernameField.placeholder = 'Not registered';
            if (usernameHint) usernameHint.textContent = 'your Cypress identity username';
        } else {
            usernameField.readOnly = false;
            usernameField.oninput = onProfileFieldChanged;
            usernameField.placeholder = 'Enter your name';
            if (usernameHint) usernameHint.textContent = 'your display name when joining servers';
        }
    }
}

function _updateIdentityButtons() {
    var games = window._identityOwnedGames || [];
    var relinked = !!window._identityEaRelinked;

    var refreshGroup = document.getElementById('refreshLicensesGroup');
    var relinkGroup = document.getElementById('relinkEaGroup');

    // show refresh licenses only if they dont have all 3 games
    if (refreshGroup) refreshGroup.style.display = (games.length < 3) ? '' : 'none';
    // show relink ea only if not already relinked
    if (relinkGroup) relinkGroup.style.display = relinked ? 'none' : '';
}

function showAuthModal() {
    document.getElementById('authModalBackdrop').style.display = 'flex';
    document.getElementById('authStatus').textContent = '';
    document.getElementById('authStatus').className = 'auth-status';
    document.getElementById('authLoginBtn').disabled = false;
}

function hideAuthModal() {
    document.getElementById('authModalBackdrop').style.display = 'none';
}

function startEaLogin() {
    const btn = document.getElementById('authLoginBtn');
    const status = document.getElementById('authStatus');
    btn.disabled = true;
    status.textContent = 'Opening EA sign-in window...';
    status.className = 'auth-status waiting';
    send('eaLogin');
}

function handleAuthStatus(data) {
    window._eaLoggedIn = !!data.loggedIn;
    updateProfileIdentitySection();
    if (data.loggedIn) {
        hideAuthModal();
        if (data.uid) setAvatarImage(data.uid);
    } else {
        showAuthModal(); // dismissable
    }
    send('init', {}); // load settings regardless of login state
}

function handleAuthLoginResult(data) {
    const btn = document.getElementById('authLoginBtn');
    const status = document.getElementById('authStatus');

    if (data.ok) {
        window._eaLoggedIn = true;
        updateProfileIdentitySection();
        status.textContent = 'Logged in as ' + data.displayName;
        status.className = 'auth-status';
        if (data.uid) setAvatarImage(data.uid);
        hideAuthModal();
        send('init', {});
    } else {
        btn.disabled = false;
        status.textContent = data.error || 'Login failed';
        status.className = 'auth-status error';
    }
}

function handleAuthLogoutResult(data) {
    if (data.ok) {
        window._eaLoggedIn = false;
        updateProfileIdentitySection();
        showAuthModal();
    }
}

// cypress identity registration

function handleIdentityStatus(data) {
    if (data.registered) {
        window._identityOwnedGames = data.ownedGames || [];
        window._identityEaRelinked = !!data.eaRelinked;
        _updateIdentityButtons();
        hideIdentityModal();
        const usernameField = document.getElementById('username');
        if (usernameField) usernameField.value = data.username || '';
        const nicknameField = document.getElementById('nicknameInput');
        if (nicknameField) nicknameField.value = data.nickname || '';
        if (typeof syncProfileDisplay === 'function') syncProfileDisplay();
    } else if (window._eaLoggedIn) {
        // not registered: confirmation with EA name w owned games
        var eaName = data.eaDisplayName || '';
        var games = data.ownedGames || [];
        showIdentityModal(eaName, games);
    }
}

function showIdentityModal(eaDisplayName, ownedGames) {
    // reset to step 1
    document.getElementById('identityConfirmStep').style.display = '';
    document.getElementById('identityUsernameStep').style.display = 'none';
    document.getElementById('identityConfirmStatus').textContent = '';
    document.getElementById('identityConfirmStatus').className = 'auth-status';
    document.getElementById('identityConfirmBtn').disabled = false;

    var nameEl = document.getElementById('identityConfirmEaName');
    if (nameEl) nameEl.textContent = eaDisplayName || '(unknown)';

    var listEl = document.getElementById('identityConfirmGamesList');
    if (listEl) {
        listEl.innerHTML = '';
        if (ownedGames && ownedGames.length > 0) {
            ownedGames.forEach(function(g) {
                var li = document.createElement('li');
                li.textContent = g;
                listEl.appendChild(li);
            });
        } else {
            var li = document.createElement('li');
            li.textContent = 'No supported games found';
            li.style.color = 'var(--text-muted, #999)';
            listEl.appendChild(li);
        }
    }

    document.getElementById('identityModalBackdrop').style.display = 'flex';
}

function hideIdentityModal() {
    document.getElementById('identityModalBackdrop').style.display = 'none';
}

function proceedToUsernameStep() {
    document.getElementById('identityConfirmStep').style.display = 'none';
    var usernameStep = document.getElementById('identityUsernameStep');
    usernameStep.style.display = '';
    document.getElementById('identityRegStatus').textContent = '';
    document.getElementById('identityRegStatus').className = 'auth-status';
    document.getElementById('identityRegisterBtn').disabled = false;
    var input = document.getElementById('identityUsernameInput');
    input.value = '';
    setTimeout(function() { input.focus(); }, 100);
}

function submitIdentityRegistration() {
    const input = document.getElementById('identityUsernameInput');
    const username = input.value.trim();
    const status = document.getElementById('identityRegStatus');
    const btn = document.getElementById('identityRegisterBtn');

    if (username.length < 3 || username.length > 32) {
        status.textContent = 'Username must be 3-32 characters';
        status.className = 'auth-status error';
        return;
    }
    if (!/^[a-zA-Z0-9][a-zA-Z0-9 _-]*[a-zA-Z0-9]$/.test(username) && !/^[a-zA-Z0-9]$/.test(username)) {
        status.textContent = 'Letters, numbers, spaces, _ and - allowed (no leading/trailing spaces)';
        status.className = 'auth-status error';
        return;
    }

    btn.disabled = true;
    status.textContent = 'Registering...';
    status.className = 'auth-status waiting';
    send('registerIdentity', { username: username });
}

function handleRegisterResult(data) {
    const status = document.getElementById('identityRegStatus');
    const btn = document.getElementById('identityRegisterBtn');

    if (data.ok) {
        status.textContent = '';
        window._identityOwnedGames = data.ownedGames || [];
        window._identityEaRelinked = false;
        _updateIdentityButtons();
        hideIdentityModal();
        const usernameField = document.getElementById('username');
        if (usernameField) usernameField.value = data.username || '';
        const nicknameField = document.getElementById('nicknameInput');
        if (nicknameField) nicknameField.value = data.nickname || '';
        updateProfileIdentitySection();
        if (typeof updateProfileWidget === 'function') updateProfileWidget();
    } else {
        btn.disabled = false;
        status.textContent = data.error || 'Registration failed';
        status.className = 'auth-status error';
    }
}

// refresh licenses

function submitRefreshEntitlements() {
    var btn = document.getElementById('refreshLicensesBtn');
    var hint = document.getElementById('refreshLicensesHint');
    if (btn) btn.disabled = true;
    if (hint) hint.textContent = 'Checking with EA...';
    send('refreshEntitlements');
}

function handleRefreshEntitlementsResult(data) {
    var btn = document.getElementById('refreshLicensesBtn');
    var hint = document.getElementById('refreshLicensesHint');
    if (data.ok) {
        window._identityOwnedGames = data.ownedGames || [];
        _updateIdentityButtons();
        if (hint) hint.textContent = 'Licenses updated';
        setTimeout(function() {
            if (hint) hint.textContent = 're-fetches your owned PvZ games from EA';
        }, 3000);
    } else {
        if (hint) hint.textContent = data.error || 'Failed to refresh';
        setTimeout(function() {
            if (hint) hint.textContent = 're-fetches your owned PvZ games from EA';
        }, 4000);
    }
    if (btn) btn.disabled = false;
}

// relink ea account

function showRelinkModal() {
    document.getElementById('relinkStatus').textContent = '';
    document.getElementById('relinkStatus').className = 'auth-status';
    document.getElementById('relinkConfirmBtn').disabled = false;
    document.getElementById('relinkModalBackdrop').style.display = 'flex';
}

function closeRelinkModal() {
    document.getElementById('relinkModalBackdrop').style.display = 'none';
}

function startRelinkEALogin() {
    var btn = document.getElementById('relinkConfirmBtn');
    var status = document.getElementById('relinkStatus');
    btn.disabled = true;
    status.textContent = 'Opening EA sign-in window...';
    status.className = 'auth-status waiting';
    send('relinkEA');
}

function handleRelinkEAResult(data) {
    var btn = document.getElementById('relinkConfirmBtn');
    var status = document.getElementById('relinkStatus');
    if (data.ok) {
        window._identityEaRelinked = true;
        window._identityOwnedGames = data.ownedGames || [];
        _updateIdentityButtons();
        closeRelinkModal();
        showStatus('EA account relinked successfully', 'info');
    } else {
        btn.disabled = false;
        status.textContent = data.error || 'Relink failed';
        status.className = 'auth-status error';
    }
}

function submitNickname() {
    var input = document.getElementById('nicknameInput');
    if (!input) return;
    var nickname = input.value.trim();
    var status = document.getElementById('nicknameStatus');
    if (nickname.length > 0 && (nickname.length < 3 || nickname.length > 32)) {
        if (status) { status.textContent = 'Nickname must be 3-32 characters or empty to clear'; status.className = 'auth-status error'; }
        return;
    }
    if (nickname.length > 0 && !/^[a-zA-Z0-9][a-zA-Z0-9 _-]*[a-zA-Z0-9]$/.test(nickname) && !/^[a-zA-Z0-9]$/.test(nickname)) {
        if (status) { status.textContent = 'Letters, numbers, spaces, _ and - allowed'; status.className = 'auth-status error'; }
        return;
    }
    if (nickname.length > 0 && Math.random() < 1 / 10000) {
        nickname = 'Cypress Carl';
    }
    if (status) { status.textContent = 'Saving...'; status.className = 'auth-status waiting'; }
    send('setNickname', { nickname: nickname });
}

function handleNicknameResult(data) {
    var status = document.getElementById('nicknameStatus');
    if (data.ok) {
        if (status) { status.textContent = data.nickname ? 'Nickname set' : 'Nickname cleared'; status.className = 'auth-status'; }
        setTimeout(function() { if (status) status.textContent = ''; }, 2000);
    } else {
        if (status) { status.textContent = data.error || 'Failed'; status.className = 'auth-status error'; }
    }
}

function clearIdentity() {
    send('clearIdentity');
}

function setAvatarImage(uid) {
    if (!uid) return;
    var url = 'https://eaavatarservice.akamaized.net/production/avatar/prod/userAvatar/' + uid + '/208x208.JPEG';
    var targets = [
        document.getElementById('profileAvatar'),
        document.getElementById('profileCardAvatar')
    ];
    targets.forEach(function(el) {
        if (!el) return;
        var img = el.querySelector('img');
        if (img) { img.src = url; return; }
        img = document.createElement('img');
        img.src = url;
        img.alt = '';
        img.onerror = function() { this.style.display = 'none'; };
        img.onload = function() { el.textContent = ''; el.appendChild(this); };
    });
}
