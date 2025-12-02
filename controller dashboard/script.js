document.addEventListener('DOMContentLoaded', () => {
    const buttons = document.querySelectorAll('.control-btn');
    const logContainer = document.getElementById('log-container');
    const clearLogsBtn = document.getElementById('clear-logs');
    const connectionStatus = document.getElementById('connectionStatus');
    const statusDot = connectionStatus.querySelector('.status-dot');
    const statusText = connectionStatus.querySelector('.status-text');

    // Simulate Hardware Connection
    setTimeout(() => {
        statusDot.style.backgroundColor = 'var(--success-color)';
        statusDot.style.boxShadow = '0 0 8px var(--success-color)';
        statusText.textContent = 'Connected';
        addLog('Hardware connected successfully.', 'system');
    }, 1500);

    // Button Event Listeners
    buttons.forEach(btn => {
        // Mouse events
        btn.addEventListener('mousedown', () => handleCommand(btn.dataset.command));
        
        // Touch events for mobile
        btn.addEventListener('touchstart', (e) => {
            e.preventDefault(); // Prevent scrolling/zooming
            handleCommand(btn.dataset.command);
        });
    });

    // Keyboard Controls
    document.addEventListener('keydown', (e) => {
        switch(e.key) {
            case 'ArrowUp':
            case 'w':
            case 'W':
                triggerButton('btn-forward');
                break;
            case 'ArrowDown':
            case 's':
            case 'S':
                triggerButton('btn-backward');
                break;
            case 'ArrowLeft':
            case 'a':
            case 'A':
                triggerButton('btn-left');
                break;
            case 'ArrowRight':
            case 'd':
            case 'D':
                triggerButton('btn-right');
                break;
        }
    });

    function triggerButton(id) {
        const btn = document.getElementById(id);
        if (btn) {
            btn.classList.add('active'); // Add active style manually if needed
            handleCommand(btn.dataset.command);
            setTimeout(() => btn.classList.remove('active'), 150);
        }
    }

    function handleCommand(command) {
        console.log(`Sending command: ${command}`);
        addLog(`Sent command: ${command}`, 'sent');
        
        // PLACEHOLDER: Hardware communication code goes here
        // Example: fetch('/api/control', { method: 'POST', body: JSON.stringify({ command }) });
    }

    function addLog(message, type = 'normal') {
        const entry = document.createElement('div');
        entry.className = `log-entry ${type}`;
        
        const timestamp = new Date().toLocaleTimeString([], { hour12: false, hour: '2-digit', minute: '2-digit', second: '2-digit' });
        entry.textContent = `[${timestamp}] ${message}`;
        
        logContainer.appendChild(entry);
        
        // Auto-scroll to bottom
        logContainer.scrollTop = logContainer.scrollHeight;
    }

    clearLogsBtn.addEventListener('click', () => {
        logContainer.innerHTML = '<div class="log-entry system">Logs cleared.</div>';
    });
});
