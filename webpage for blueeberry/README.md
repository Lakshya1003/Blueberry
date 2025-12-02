# Project Blueberry Website

Marketing and information site for **Project Blueberry — Autonomous Robotics for Disaster Response**.

## Overview
This is a single-page application built with vanilla HTML, CSS, and JavaScript. It features a futuristic design with 3D elements powered by Three.js and complex animations using Anime.js.

## Tech Stack
- **HTML5**: Semantic structure.
- **CSS3**: Custom variables, Flexbox/Grid, Glassmorphism effects.
- **JavaScript (ES6+)**: Modular logic.
- **Three.js (r128)**: 3D hero visualization.
- **Anime.js (3.2.1)**: UI animations and transitions.

## Build & Run
Since this project uses vanilla web technologies and CDNs, no build step is required.

1. **Clone the repository** (or download the source files).
2. **Open `index.html`** in any modern web browser.
   - *Note*: For correct loading of ES modules or textures (if added later), it is recommended to use a local development server (e.g., Live Server in VS Code, or `python -m http.server`).

## Live Telemetry Integration
The "Live Demo" section is set up to display real-time data. Currently, it uses mock data generated in `app.js`.

To connect to a real backend:
1. Open `app.js`.
2. Locate the `connectBtn` event listener.
3. Replace the `setInterval` mock logic with a `fetch` or `WebSocket` call to your API.

**Example Fetch Implementation:**
```javascript
async function fetchTelemetry() {
    try {
        const response = await fetch('/api/telemetry/latest');
        const data = await response.json();
        
        // Update UI
        document.getElementById('battery-val').textContent = `${data.battery}%`;
        document.getElementById('dist-val').textContent = `${data.distance} cm`;
        
        // Update Status
        const statusEl = document.getElementById('status-val');
        statusEl.textContent = data.online ? "ONLINE" : "OFFLINE";
        statusEl.style.color = data.online ? "#00E5A8" : "#FF5555";
        
    } catch (error) {
        console.error("Telemetry Error:", error);
    }
}

// Poll every 2 seconds
setInterval(fetchTelemetry, 2000);
```

## Accessibility
- **Semantic HTML**: Used `<main>`, `<section>`, `<nav>`, `<footer>` for proper document structure.
- **Contrast**: High contrast colors (Cyan/White on Dark Navy) meet WCAG AA standards.
- **Keyboard Navigation**: Interactive elements (Buttons, Roadmap Stages) are focusable.
- **Alt Text**: Ensure any future images added have descriptive `alt` attributes.
- **Reduced Motion**: Critical animations respect `prefers-reduced-motion` (can be enhanced in CSS).

## Credits
- **Design & Dev**: Project Blueberry Team
- **Libraries**: Three.js, Anime.js, Google Fonts
