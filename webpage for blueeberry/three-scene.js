// Three.js Scene for Project Blueberry Hero Section

document.addEventListener('DOMContentLoaded', () => {
  const container = document.getElementById('hero-3d-container')
  if (!container) return

  // Scene Setup
  const scene = new THREE.Scene()

  // Camera
  const camera = new THREE.PerspectiveCamera(
    75,
    container.clientWidth / container.clientHeight,
    0.1,
    1000
  )
  camera.position.z = 5

  // Renderer
  const renderer = new THREE.WebGLRenderer({ alpha: true, antialias: true })
  renderer.setSize(container.clientWidth, container.clientHeight)
  renderer.setPixelRatio(window.devicePixelRatio)
  container.appendChild(renderer.domElement)

  // Objects
  // 1. Main Wireframe Sphere (The "Core")
  const geometry = new THREE.IcosahedronGeometry(2, 2)
  const material = new THREE.MeshBasicMaterial({
    color: 0x00e5a8,
    wireframe: true,
    transparent: true,
    opacity: 0.3,
  })
  const sphere = new THREE.Mesh(geometry, material)
  scene.add(sphere)

  // 2. Inner Glowing Core
  const coreGeometry = new THREE.IcosahedronGeometry(1, 1)
  const coreMaterial = new THREE.MeshBasicMaterial({
    color: 0x2ea6ff,
    wireframe: true,
    transparent: true,
    opacity: 0.5,
  })
  const core = new THREE.Mesh(coreGeometry, coreMaterial)
  scene.add(core)

  // --- Anime.js Integrations ---

  // 0. Initial State (Hidden for Intro)
  sphere.scale.set(0, 0, 0);
  core.scale.set(0, 0, 0);
  material.opacity = 0;
  coreMaterial.opacity = 0;

  // 1. Intro Animation
  const introTimeline = anime.timeline({
    easing: 'easeOutElastic(1, .8)',
    duration: 2000
  });

  introTimeline
    .add({
      targets: [sphere.scale, core.scale],
      x: 1,
      y: 1,
      z: 1,
      delay: anime.stagger(200)
    })
    .add({
      targets: [material, coreMaterial],
      opacity: [0, (el, i) => i === 0 ? 0.3 : 0.5], // Restore original opacities
      duration: 1000,
      easing: 'linear'
    }, '-=1800');

  // 2. Continuous Floating / Breathing
  anime({
    targets: core.scale,
    x: 1.1,
    y: 1.1,
    z: 1.1,
    direction: 'alternate',
    loop: true,
    duration: 2000,
    easing: 'easeInOutSine'
  });

  anime({
    targets: sphere.position,
    y: 0.2,
    direction: 'alternate',
    loop: true,
    duration: 3000,
    easing: 'easeInOutQuad'
  });

  // 3. Interaction (Pulse on Click)
  container.addEventListener('click', () => {
    anime({
      targets: core.scale,
      x: [1.5, 1],
      y: [1.5, 1],
      z: [1.5, 1],
      duration: 800,
      easing: 'easeOutElastic(1, .5)'
    });

    // Flash color
    const oldColor = coreMaterial.color.getHex();
    const flashColor = { r: 1, g: 1, b: 1 }; // White flash logic if needed, or just intensity

    // Simple opacity flash
    anime({
      targets: coreMaterial,
      opacity: [0.8, 0.5],
      duration: 500,
      easing: 'easeOutQuad'
    });
  });

  // 3. Particle Field
  const particlesGeometry = new THREE.BufferGeometry()
  const particlesCount = 500
  const posArray = new Float32Array(particlesCount * 3)

  for (let i = 0; i < particlesCount * 3; i++) {
    posArray[i] = (Math.random() - 0.5) * 10
  }

  particlesGeometry.setAttribute(
    'position',
    new THREE.BufferAttribute(posArray, 3)
  )
  const particlesMaterial = new THREE.PointsMaterial({
    size: 0.02,
    color: 0xff9f1c,
    transparent: true,
    opacity: 0.8,
  })
  const particlesMesh = new THREE.Points(particlesGeometry, particlesMaterial)
  scene.add(particlesMesh)

  // Animation Loop
  let mouseX = 0
  let mouseY = 0

  // Mouse Interaction
  document.addEventListener('mousemove', (event) => {
    mouseX = (event.clientX / window.innerWidth) * 2 - 1
    mouseY = -(event.clientY / window.innerHeight) * 2 + 1
  })

  const animate = () => {
    requestAnimationFrame(animate)

    // Rotation
    sphere.rotation.y += 0.002
    sphere.rotation.x += 0.001

    core.rotation.y -= 0.005
    core.rotation.x -= 0.002

    particlesMesh.rotation.y += 0.0005

    // Mouse Parallax
    sphere.rotation.y += mouseX * 0.01
    sphere.rotation.x += mouseY * 0.01

    renderer.render(scene, camera)
  }

  animate()

  // Resize Handler
  window.addEventListener('resize', () => {
    camera.aspect = container.clientWidth / container.clientHeight
    camera.updateProjectionMatrix()
    renderer.setSize(container.clientWidth, container.clientHeight)
  })
})
