// Project Blueberry Main Application Logic

document.addEventListener('DOMContentLoaded', () => {

    // --- Particles.js Init ---
    if (window.particlesJS) {
        particlesJS('particles-js', {
            "particles": {
                "number": { "value": 60, "density": { "enable": true, "value_area": 800 } },
                "color": { "value": "#00E5A8" },
                "shape": { "type": "circle" },
                "opacity": { "value": 0.3, "random": false },
                "size": { "value": 2, "random": true },
                "line_linked": { "enable": true, "distance": 150, "color": "#2EA6FF", "opacity": 0.2, "width": 1 },
                "move": { "enable": true, "speed": 1, "direction": "none", "random": false, "straight": false, "out_mode": "out", "bounce": false }
            },
            "interactivity": {
                "detect_on": "canvas",
                "events": {
                    "onhover": { "enable": true, "mode": "bubble" },
                    "onclick": { "enable": true, "mode": "push" },
                    "resize": true
                },
                "modes": {
                    "bubble": { "distance": 200, "size": 4, "duration": 2, "opacity": 0.8 },
                    "push": { "particles_nb": 4 }
                }
            },
            "retina_detect": true
        });
    }

    // --- Animations (Anime.js) ---

    // Hero Entry Animation
    const heroTimeline = anime.timeline({
        easing: 'easeOutExpo',
        duration: 1000
    });

    // Nav Links Entry
    heroTimeline.add({
        targets: '.nav-links a, .logo',
        translateY: [-20, 0],
        opacity: [0, 1],
        delay: anime.stagger(100)
    });

    heroTimeline
        .add({
            targets: '.hero-title .line',
            translateY: [50, 0],
            opacity: [0, 1],
            delay: anime.stagger(200)
        })
        .add({
            targets: '.hero-subtitle',
            opacity: [0, 1],
            translateY: [20, 0]
        }, '-=500')
        .add({
            targets: '.hero-summary',
            opacity: [0, 1],
            translateY: [20, 0]
        }, '-=600')
        .add({
            targets: '.hero-cta-group .btn',
            scale: [0.8, 1],
            opacity: [0, 1],
            delay: anime.stagger(100)
        }, '-=600')
        .add({
            targets: '#hero-3d-container',
            opacity: [0, 1],
            scale: [0.8, 1],
            duration: 1500
        }, '-=1000');

    // Scroll Animations (Intersection Observer)
    const observerOptions = {
        threshold: 0.1,
        rootMargin: "0px 0px -50px 0px"
    };

    const observer = new IntersectionObserver((entries) => {
        entries.forEach(entry => {
            if (entry.isIntersecting) {
                entry.target.classList.add('in-view');

                // Animate Cards (Problem Section)
                if (entry.target.classList.contains('card-grid')) {
                    anime({
                        targets: entry.target.querySelectorAll('.card'),
                        translateY: [50, 0],
                        opacity: [0, 1],
                        delay: anime.stagger(150),
                        easing: 'easeOutQuad'
                    });
                }

                // Animate Evolution Stages
                if (entry.target.classList.contains('stages-container')) {
                    anime({
                        targets: entry.target.querySelectorAll('.stage-card'),
                        translateY: [50, 0],
                        opacity: [0, 1],
                        delay: anime.stagger(200),
                        easing: 'easeOutQuad'
                    });
                }

                // Animate Tech Lists
                if (entry.target.classList.contains('tech-grid')) {
                    anime({
                        targets: entry.target.querySelectorAll('li'),
                        translateX: [-20, 0],
                        opacity: [0, 1],
                        delay: anime.stagger(50),
                        easing: 'easeOutQuad'
                    });
                }

                // Animate Use Cases
                if (entry.target.classList.contains('grid-3')) {
                    anime({
                        targets: entry.target.querySelectorAll('.use-case-item'),
                        translateY: [30, 0],
                        opacity: [0, 1],
                        delay: anime.stagger(150),
                        easing: 'easeOutQuad'
                    });
                }

                // Animate Team
                if (entry.target.classList.contains('team-grid')) {
                    anime({
                        targets: entry.target.querySelectorAll('.team-member'),
                        scale: [0.9, 1],
                        opacity: [0, 1],
                        delay: anime.stagger(150),
                        easing: 'easeOutBack'
                    });
                }

                observer.unobserve(entry.target);
            }
        });
    }, observerOptions);

    document.querySelectorAll('.card-grid, .stages-container, .tech-grid, .grid-3, .team-grid, .section-heading').forEach(el => {
        observer.observe(el);
    });


    // --- Button Hover Effects (Magnetic/Tech feel) ---
    document.querySelectorAll('.btn').forEach(btn => {
        btn.addEventListener('mouseenter', () => {
            anime({
                targets: btn,
                scale: 1.05,
                duration: 300,
                easing: 'easeOutQuad'
            });
        });
        btn.addEventListener('mouseleave', () => {
            anime({
                targets: btn,
                scale: 1,
                duration: 300,
                easing: 'easeOutQuad'
            });
        });
    });


    // --- Mock Telemetry & Radar ---
    const statusVal = document.getElementById('status-val');
    const batteryVal = document.getElementById('battery-val');
    const distVal = document.getElementById('dist-val');
    const connectBtn = document.getElementById('connect-btn');
    const radarCanvas = document.getElementById('radarCanvas');

    if (radarCanvas) {
        const ctx = radarCanvas.getContext('2d');

        // Resize Canvas
        function resizeCanvas() {
            radarCanvas.width = radarCanvas.parentElement.clientWidth;
            radarCanvas.height = radarCanvas.parentElement.clientHeight;
        }
        window.addEventListener('resize', resizeCanvas);
        resizeCanvas();

        // Radar Animation
        let radarAngle = 0;
        const points = [];

        // Generate random points
        for (let i = 0; i < 10; i++) {
            points.push({
                x: Math.random() * radarCanvas.width,
                y: Math.random() * radarCanvas.height,
                life: Math.random()
            });
        }

        function drawRadar() {
            ctx.clearRect(0, 0, radarCanvas.width, radarCanvas.height);

            // Draw Scan Line
            const cx = radarCanvas.width / 2;
            const cy = radarCanvas.height / 2;
            const radius = Math.min(cx, cy) - 10;

            ctx.strokeStyle = 'rgba(0, 229, 168, 0.3)';
            ctx.lineWidth = 1;
            ctx.beginPath();
            ctx.arc(cx, cy, radius, 0, Math.PI * 2);
            ctx.stroke();

            // Scan Sweep
            ctx.save();
            ctx.translate(cx, cy);
            ctx.rotate(radarAngle);
            ctx.beginPath();
            ctx.moveTo(0, 0);
            ctx.lineTo(0, -radius);
            ctx.strokeStyle = 'rgba(0, 229, 168, 0.8)';
            ctx.stroke();

            // Gradient Sweep
            const gradient = ctx.createLinearGradient(0, 0, 0, -radius);
            gradient.addColorStop(0, 'rgba(0, 229, 168, 0)');
            gradient.addColorStop(1, 'rgba(0, 229, 168, 0.15)');
            ctx.fillStyle = gradient;
            ctx.beginPath();
            ctx.moveTo(0, 0);
            ctx.arc(0, 0, radius, -Math.PI / 2, -Math.PI / 2 + 0.5, false);
            ctx.fill();

            ctx.restore();

            radarAngle += 0.04;

            // Draw Points (Mock Obstacles)
            ctx.fillStyle = '#FF9F1C';
            points.forEach(p => {
                ctx.globalAlpha = Math.abs(Math.sin(radarAngle + p.life));
                ctx.beginPath();
                ctx.arc(p.x, p.y, 2, 0, Math.PI * 2);
                ctx.fill();
            });
            ctx.globalAlpha = 1;

            requestAnimationFrame(drawRadar);
        }

        drawRadar();
    }

    // Mock Data Update
    let isConnected = false;
    let intervalId;

    if (connectBtn) {
        connectBtn.addEventListener('click', () => {
            if (!isConnected) {
                isConnected = true;
                connectBtn.textContent = "Disconnect";
                connectBtn.classList.replace('btn-primary', 'btn-secondary');
                document.querySelector('.overlay-text').style.display = 'none';

                intervalId = setInterval(() => {
                    // Update Battery
                    const bat = Math.floor(80 + Math.random() * 20);
                    batteryVal.textContent = `${bat}%`;

                    // Update Distance
                    const dist = Math.floor(Math.random() * 500);
                    distVal.textContent = `${dist} cm`;

                    // Update Status
                    statusVal.textContent = "ACTIVE SCANNING";
                    statusVal.style.color = "#00E5A8";

                }, 2000);
            } else {
                isConnected = false;
                connectBtn.textContent = "Connect Stream";
                connectBtn.classList.replace('btn-secondary', 'btn-primary');
                document.querySelector('.overlay-text').style.display = 'block';
                clearInterval(intervalId);
                statusVal.textContent = "STANDBY";
                statusVal.style.color = "#E6EEF6";
            }
        });
    }

});
