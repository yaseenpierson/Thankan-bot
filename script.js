/* ============================================================
   THANKAN-BOT: Technical Field Report & Experiment Engine
   Case File: THK-BOT-001 // TinkerHub Useless Projects 3.0
   ============================================================ */

document.addEventListener('DOMContentLoaded', () => {
  // Global Audio Context (synthesized test tones, no external file dependencies)
  let audioCtx = null;
  function getAudioContext() {
    if (!audioCtx) {
      const AudioContext = window.AudioContext || window.webkitAudioContext;
      audioCtx = new AudioContext();
    }
    if (audioCtx.state === 'suspended') {
      audioCtx.resume();
    }
    return audioCtx;
  }

  // ============================================================
  // PROCEDURAL SOUND SYNTHESIZER
  // ============================================================
  const SoundFX = {
    playEvasionScreech() {
      try {
        const ctx = getAudioContext();
        const osc = ctx.createOscillator();
        const gain = ctx.createGain();
        osc.type = 'sawtooth';

        const now = ctx.currentTime;
        osc.frequency.setValueAtTime(850, now);
        osc.frequency.exponentialRampToValueAtTime(160, now + 0.32);

        gain.gain.setValueAtTime(0.25, now);
        gain.gain.exponentialRampToValueAtTime(0.01, now + 0.32);

        osc.connect(gain);
        gain.connect(ctx.destination);

        osc.start(now);
        osc.stop(now + 0.32);
      } catch (e) {
        console.warn('Audio play failed:', e);
      }
    },

    playSayippHonk() {
      try {
        const ctx = getAudioContext();
        const osc = ctx.createOscillator();
        const gain = ctx.createGain();
        osc.type = 'square';

        const now = ctx.currentTime;
        osc.frequency.setValueAtTime(320, now);
        osc.frequency.setValueAtTime(460, now + 0.1);
        osc.frequency.setValueAtTime(320, now + 0.2);

        gain.gain.setValueAtTime(0.2, now);
        gain.gain.linearRampToValueAtTime(0.01, now + 0.35);

        osc.connect(gain);
        gain.connect(ctx.destination);

        osc.start(now);
        osc.stop(now + 0.35);
      } catch (e) {
        console.warn('Audio play failed:', e);
      }
    },

    playTrollBoing() {
      try {
        const ctx = getAudioContext();
        const osc = ctx.createOscillator();
        const gain = ctx.createGain();
        osc.type = 'sine';

        const now = ctx.currentTime;
        osc.frequency.setValueAtTime(130, now);
        osc.frequency.exponentialRampToValueAtTime(580, now + 0.22);

        gain.gain.setValueAtTime(0.25, now);
        gain.gain.exponentialRampToValueAtTime(0.01, now + 0.25);

        osc.connect(gain);
        gain.connect(ctx.destination);

        osc.start(now);
        osc.stop(now + 0.25);
      } catch (e) {
        console.warn('Audio play failed:', e);
      }
    },

    playAlarmSiren() {
      try {
        const ctx = getAudioContext();
        const osc = ctx.createOscillator();
        const gain = ctx.createGain();
        osc.type = 'triangle';

        const now = ctx.currentTime;
        for (let i = 0; i < 4; i++) {
          osc.frequency.setValueAtTime(750, now + (i * 0.08));
          osc.frequency.setValueAtTime(1150, now + (i * 0.08) + 0.04);
        }

        gain.gain.setValueAtTime(0.18, now);
        gain.gain.linearRampToValueAtTime(0.01, now + 0.4);

        osc.connect(gain);
        gain.connect(ctx.destination);

        osc.start(now);
        osc.stop(now + 0.4);
      } catch (e) {
        console.warn('Audio play failed:', e);
      }
    }
  };

  // High-pitch deadpan robotic voice synthesis
  function speakMemeDialogue(text) {
    if ('speechSynthesis' in window) {
      window.speechSynthesis.cancel();
      const utterance = new SpeechSynthesisUtterance(text);
      utterance.rate = 1.15;
      utterance.pitch = 1.35;
      window.speechSynthesis.speak(utterance);
    }
  }

  // ============================================================
  // PROCEDURAL OLED EYE ENGINE (SSD1306 Replication from Bot.ino)
  // ============================================================
  const oledCanvas = document.getElementById('oledCanvas');
  const moodLabel = document.getElementById('oledMoodLabel');
  let oledCtx = null;
  if (oledCanvas) {
    oledCtx = oledCanvas.getContext('2d');
  }

  const EYE_WIDTH = 56;
  const EYE_HEIGHT = 56;
  const EYE_SPACING = 24;
  const CORNER_RADIUS = 12;

  let currentOledMode = 'IDLE'; // IDLE, HAPPY, SURPRISED, ANGRY, CONFUSED, SLEEPY, WINK
  let blinkProgress = 0;
  let isBlinking = false;
  let breathPhase = 0;
  let lastBlinkTime = Date.now();
  let nextBlinkInterval = 3200;

  // Eye cursor tracking offsets
  let eyePupilOffsetX = 0;
  let eyePupilOffsetY = 0;

  window.addEventListener('mousemove', (e) => {
    if (oledCanvas) {
      const rect = oledCanvas.getBoundingClientRect();
      const dx = (e.clientX - (rect.left + rect.width / 2)) / (rect.width || 1);
      const dy = (e.clientY - (rect.top + rect.height / 2)) / (rect.height || 1);
      eyePupilOffsetX = Math.max(-6, Math.min(6, dx * 10));
      eyePupilOffsetY = Math.max(-4, Math.min(4, dy * 8));
    }
  });

  function drawRoundedRect(ctx, x, y, width, height, radius) {
    const r = Math.max(0, Math.min(radius, Math.abs(width) / 2, Math.abs(height) / 2));
    ctx.beginPath();
    if (typeof ctx.roundRect === 'function') {
      ctx.roundRect(x, y, width, height, r);
    } else {
      ctx.moveTo(x + r, y);
      ctx.lineTo(x + width - r, y);
      ctx.arc(x + width - r, y + r, r, -Math.PI / 2, 0);
      ctx.lineTo(x + width, y + height - r);
      ctx.arc(x + width - r, y + height - r, r, 0, Math.PI / 2);
      ctx.lineTo(x + r, y + height);
      ctx.arc(x + r, y + height - r, r, Math.PI / 2, Math.PI);
      ctx.lineTo(x, y + r);
      ctx.arc(x + r, y + r, r, Math.PI, -Math.PI / 2);
      ctx.closePath();
    }
    ctx.fill();
  }

  function adjustCanvasSize() {
    if (!oledCanvas) return;
    const w = oledCanvas.clientWidth || 480;
    const h = oledCanvas.clientHeight || 200;
    if (oledCanvas.width !== w || oledCanvas.height !== h) {
      oledCanvas.width = w;
      oledCanvas.height = h;
    }
  }
  adjustCanvasSize();
  window.addEventListener('resize', adjustCanvasSize);

  function renderOledEyes() {
    if (!oledCtx) return;
    const width = oledCanvas.width;
    const height = oledCanvas.height;

    // Clear OLED background
    oledCtx.fillStyle = '#050b14';
    oledCtx.fillRect(0, 0, width, height);

    // Subtle OLED scanline texture
    oledCtx.fillStyle = 'rgba(0, 210, 255, 0.03)';
    for (let y = 0; y < height; y += 4) {
      oledCtx.fillRect(0, y, width, 2);
    }

    // Sinusoidal breathing offset
    breathPhase += 0.04;
    const breathOffset = Math.sin(breathPhase) * 3;

    // Periodic eyelid blink calculation
    const now = Date.now();
    if (!isBlinking && now - lastBlinkTime > nextBlinkInterval) {
      isBlinking = true;
      blinkProgress = 0;
    }

    let blinkScaleY = 1;
    if (isBlinking) {
      blinkProgress += 0.16;
      blinkScaleY = Math.abs(Math.cos(blinkProgress * Math.PI));
      if (blinkProgress >= 1) {
        isBlinking = false;
        lastBlinkTime = now;
        nextBlinkInterval = 2200 + Math.random() * 3500;
        blinkScaleY = 1;
      }
    }

    const centerX = width / 2 + (currentOledMode === 'IDLE' ? eyePupilOffsetX : 0);
    const centerY = height / 2 + (currentOledMode === 'IDLE' ? eyePupilOffsetY : 0);

    oledCtx.fillStyle = '#00d2ff';
    oledCtx.shadowColor = '#00d2ff';
    oledCtx.shadowBlur = 14;

    let lWidth = EYE_WIDTH;
    let lHeight = (EYE_HEIGHT + breathOffset) * blinkScaleY;
    let rWidth = EYE_WIDTH;
    let rHeight = (EYE_HEIGHT + breathOffset) * blinkScaleY;

    let leftX = centerX - EYE_SPACING / 2 - lWidth;
    let rightX = centerX + EYE_SPACING / 2;
    let leftY = centerY - lHeight / 2;
    let rightY = centerY - rHeight / 2;

    switch (currentOledMode) {
      case 'HAPPY':
        // Curved inverted crescents
        oledCtx.beginPath();
        oledCtx.arc(leftX + lWidth/2, leftY + lHeight/2, lWidth/2, Math.PI, 0, false);
        oledCtx.lineTo(leftX + lWidth, leftY + lHeight/2 + 10);
        oledCtx.arc(leftX + lWidth/2, leftY + lHeight/2 + 10, lWidth/2, 0, Math.PI, true);
        oledCtx.closePath();
        oledCtx.fill();

        oledCtx.beginPath();
        oledCtx.arc(rightX + rWidth/2, rightY + rHeight/2, rWidth/2, Math.PI, 0, false);
        oledCtx.lineTo(rightX + rWidth, rightY + rHeight/2 + 10);
        oledCtx.arc(rightX + rWidth/2, rightY + rHeight/2 + 10, rWidth/2, 0, Math.PI, true);
        oledCtx.closePath();
        oledCtx.fill();
        break;

      case 'SURPRISED':
        // Big round alert circles
        oledCtx.fillStyle = '#38bdf8';
        oledCtx.shadowColor = '#38bdf8';
        oledCtx.shadowBlur = 24;
        drawRoundedRect(oledCtx, leftX - 4, leftY - 8, lWidth + 8, lHeight + 16, 24);
        drawRoundedRect(oledCtx, rightX - 4, rightY - 8, rWidth + 8, rHeight + 16, 24);
        break;

      case 'ANGRY':
        // Slanted inward menacing eyes (Red hue)
        oledCtx.fillStyle = '#f85149';
        oledCtx.shadowColor = '#f85149';
        oledCtx.shadowBlur = 18;

        oledCtx.beginPath();
        oledCtx.moveTo(leftX, leftY + 12);
        oledCtx.lineTo(leftX + lWidth, leftY);
        oledCtx.lineTo(leftX + lWidth, leftY + lHeight);
        oledCtx.lineTo(leftX, leftY + lHeight - 6);
        oledCtx.closePath();
        oledCtx.fill();

        oledCtx.beginPath();
        oledCtx.moveTo(rightX, rightY);
        oledCtx.lineTo(rightX + rWidth, rightY + 12);
        oledCtx.lineTo(rightX + rWidth, rightY + rHeight - 6);
        oledCtx.lineTo(rightX, rightY + rHeight);
        oledCtx.closePath();
        oledCtx.fill();
        break;

      case 'CONFUSED':
        // Asymmetrical eye scales
        drawRoundedRect(oledCtx, leftX - 6, leftY - 10, lWidth + 12, lHeight + 15, CORNER_RADIUS);
        drawRoundedRect(oledCtx, rightX + 6, rightY + 12, rWidth - 12, (rHeight - 15) * 0.5, CORNER_RADIUS / 2);
        break;

      case 'SLEEPY':
        // Narrow horizontal slits
        drawRoundedRect(oledCtx, leftX, centerY - 5, lWidth, 10, 3);
        drawRoundedRect(oledCtx, rightX, centerY - 5, rWidth, 10, 3);
        break;

      case 'WINK':
        drawRoundedRect(oledCtx, leftX, leftY, lWidth, lHeight, CORNER_RADIUS);
        oledCtx.lineWidth = 6;
        oledCtx.strokeStyle = '#00d2ff';
        oledCtx.beginPath();
        oledCtx.moveTo(rightX, centerY);
        oledCtx.lineTo(rightX + rWidth, centerY);
        oledCtx.stroke();
        break;

      case 'IDLE':
      default:
        drawRoundedRect(oledCtx, leftX, leftY, lWidth, lHeight, CORNER_RADIUS);
        drawRoundedRect(oledCtx, rightX, rightY, rWidth, rHeight, CORNER_RADIUS);
        break;
    }

    oledCtx.shadowBlur = 0;
    requestAnimationFrame(renderOledEyes);
  }

  if (oledCanvas) {
    renderOledEyes();
  }

  // OLED mode button controls
  const oledToggles = document.querySelectorAll('.psych-mode-btn, .oled-mode-toggle');
  const moodDescriptions = {
    'IDLE': 'STATE: MODE_IDLE [Breathing sinusoid: 0.04 Hz, periodic blink active]',
    'HAPPY': 'STATE: MODE_HAPPY [Inverted arcs: Target not attempting to drink]',
    'SURPRISED': 'STATE: MODE_SURPRISED [Rising-edge trigger on IR-01: Hand detected]',
    'ANGRY': 'STATE: MODE_ANGRY [Slanted fury: Reverse escape thrusters at 100% PWM]',
    'CONFUSED': 'STATE: MODE_CONFUSED [Asymmetric pupils: Multi-sensor ambiguity]',
    'SLEEPY': 'STATE: MODE_SLEEPY [Slits: Zero threats in sector for >120 seconds]',
    'WINK': 'STATE: MODE_WINK [Thankan Chettan protocol: Scene contra confirmation]'
  };

  oledToggles.forEach(btn => {
    btn.addEventListener('click', () => {
      oledToggles.forEach(b => b.classList.remove('active'));
      btn.classList.add('active');
      currentOledMode = btn.dataset.mode;
      if (moodLabel) {
        moodLabel.textContent = moodDescriptions[currentOledMode] || `STATE: MODE_${currentOledMode}`;
      }
      if (currentOledMode === 'ANGRY') {
        SoundFX.playAlarmSiren();
      } else if (currentOledMode === 'SURPRISED') {
        SoundFX.playEvasionScreech();
      } else {
        SoundFX.playTrollBoing();
      }
    });
  });

  // ============================================================
  // EXPERIMENT 001: LABORATORY TEST BENCH
  // ============================================================
  const benchArena = document.getElementById('benchArena');
  const benchBot = document.getElementById('benchBot');
  const benchDisc = document.getElementById('benchDisc');
  const benchHand = document.getElementById('benchHand');
  const benchSpeech = document.getElementById('benchSpeech');
  const benchOledEyes = document.getElementById('benchOledEyes');

  // Telemetry DOM elements
  const telemetryDist = document.getElementById('telemetryDist');
  const telemetrySensor = document.getElementById('telemetrySensor');
  const telemetryThreat = document.getElementById('telemetryThreat');
  const telemetryVector = document.getElementById('telemetryVector');
  const telemetryMotor = document.getElementById('telemetryMotor');
  const telemetryEscapes = document.getElementById('telemetryEscapes');
  const telemetryStatusBadge = document.getElementById('telemetryStatusBadge');
  const consoleLogBox = document.getElementById('consoleLogBox');

  let botX = 260;
  let botY = 210;
  let isEvading = false;
  let escapeCount = 0;
  let speechTimeout = null;

  function addConsoleLog(msg, type = 'info') {
    if (!consoleLogBox) return;
    const now = new Date();
    const ts = now.toTimeString().split(' ')[0] + '.' + String(now.getMilliseconds()).padStart(3, '0');
    const line = document.createElement('div');
    line.className = `console-log-line ${type}`;
    line.textContent = `[${ts}] ${msg}`;
    consoleLogBox.appendChild(line);
    consoleLogBox.scrollTop = consoleLogBox.scrollHeight;
  }

  const deadpanQuotes = [
    { text: "ഏതാ പുണ്ണകളെ... തങ്കൻ ഞാനാടാ പൂറി മോനെ!", voice: "Etha punnakale! Thankan njanada poori mone!" },
    { text: "തങ്ങാട്ടന്റെ അണ്ടി നീ! ഏതാടാ പുണ്ണെ ഫോൺ വിളിച്ചേ!", voice: "Thankante andi nee! Ethaada punne!" },
    { text: "എടാ പോടാ കൂണേ നീ എന്റെ അണ്ടി തിന്നടാ!", voice: "Eda poda koone nee ente andi thinnada!" },
    { text: "കുണ വന്നിട്ട് തണ്ട ഇല്ലാത്തവനെ പുണ്ടച്ചു മോനെ!", voice: "Kuna vannittu thanda illathavane pundachu mone!" },
    { text: "കൂണ്ണകളെ ഞാനൊരു മലയാളിയല്ലടാ... മൈരേ!", voice: "Kunnagale njanoru Malayali allada maire!" },
    { text: "പോളയടി മോനേ! അടിച്ച് കരണം ഞാൻ പൊട്ടിക്കും!", voice: "Polayadi mone! Adichu karanam njan pottikkum!" },
    { text: "നിന്റെ മോന്തായം ഇടിച്ചു ഞാൻ പറത്തും!", voice: "Ninte monthayam idichu njan parathum!" },
    { text: "താനാരാണേലും എനിക്ക് പുല്ലാ... മതിയായെടോ ഈ തോട്ടിപ്പണി!", voice: "Thanaaraanelem enikku pulla! Mathiyayedo ee thottippani!" },
    { text: "നീ പോ മോനേ ദിനേശാ... നിന്റെ തായോളി കളി വേണ്ട!", voice: "Nee po mone Dinesha! Thayoli kali ivide venda!" },
    { text: "ലാലേട്ടൻ ആറാടുകയാണ്... ഏത് പുണ്ണനാടാ ഇത്!", voice: "Lalettan aaraadukayanu! Ethu punnanada ithu!" },
    { text: "നിന്റെ അച്ഛന്റെ അണ്ടി... തീട്ടം... വാണം... ഊമ്പി!", voice: "Ninte achante andi! Theettam! Vaanam! Oombi!" }
  ];

  function showSpeech(text, x, y) {
    if (!benchSpeech) return;
    benchSpeech.textContent = text;
    benchSpeech.style.left = `${x}px`;
    benchSpeech.style.top = `${y}px`;
    benchSpeech.style.opacity = '1';

    clearTimeout(speechTimeout);
    speechTimeout = setTimeout(() => {
      benchSpeech.style.opacity = '0';
    }, 2000);
  }

  function triggerLabEvade(handX, handY, sensorTag = 'IR-01') {
    if (isEvading) return;
    isEvading = true;

    escapeCount++;
    if (telemetryEscapes) telemetryEscapes.textContent = escapeCount;

    // Play escape sound
    SoundFX.playEvasionScreech();

    // Pick deadpan quote
    const quote = deadpanQuotes[Math.floor(Math.random() * deadpanQuotes.length)];
    speakMemeDialogue(quote.voice);

    // Calculate evasion vector
    const dx = botX - handX;
    const dy = botY - handY;
    const dist = Math.sqrt(dx * dx + dy * dy) || 1;
    const angleRad = Math.atan2(dy, dx);
    const angleDeg = ((angleRad * 180 / Math.PI) + 360) % 360;

    // Telemetry updates
    if (telemetrySensor) telemetrySensor.textContent = sensorTag;
    if (telemetryThreat) {
      telemetryThreat.textContent = 'CRITICAL (EVADE)';
      telemetryThreat.className = 't-val danger';
    }
    if (telemetryVector) telemetryVector.textContent = `${angleDeg.toFixed(1)}° ESCAPE`;
    if (telemetryMotor) telemetryMotor.textContent = '100% PWM (FULL REVERSE)';
    if (telemetryStatusBadge) {
      telemetryStatusBadge.textContent = 'EVADING';
      telemetryStatusBadge.className = 'tech-label red';
    }

    addConsoleLog(`THREAT TRIGGER: ${sensorTag} // ANGLE: ${angleDeg.toFixed(0)}°`, 'alert');
    addConsoleLog(`MOTOR PWM: 100% // ESCAPE VECTOR ENGAGED`, 'warn');

    // Visual feedback
    if (benchDisc) benchDisc.classList.add('panic');
    if (benchOledEyes) benchOledEyes.textContent = '> <';

    // Push bot further away
    const fleeDistance = 160 + Math.random() * 70;
    let newX = botX + (dx / dist) * fleeDistance;
    let newY = botY + (dy / dist) * fleeDistance;

    const pad = 65;
    const maxX = benchArena.clientWidth - pad;
    const maxY = benchArena.clientHeight - pad;

    // Bounce if hitting boundary
    if (newX < pad || newX > maxX || newY < pad || newY > maxY) {
      newX = pad + Math.random() * (maxX - pad);
      newY = pad + Math.random() * (maxY - pad);
    }

    botX = newX;
    botY = newY;

    if (benchBot) {
      benchBot.style.left = `${botX}px`;
      benchBot.style.top = `${botY}px`;
      benchBot.style.transform = `translate(-50%, -50%) rotate(${Math.random() * 30 - 15}deg)`;
    }

    showSpeech(quote.text, botX, botY - 60);

    // Sync OLED Showcase to ANGRY
    currentOledMode = 'ANGRY';
    oledToggles.forEach(b => {
      b.classList.toggle('active', b.dataset.mode === 'ANGRY');
    });
    if (moodLabel) moodLabel.textContent = moodDescriptions['ANGRY'];

    setTimeout(() => {
      isEvading = false;
      if (benchDisc) benchDisc.classList.remove('panic');
      if (benchOledEyes) benchOledEyes.textContent = '•  •';

      if (telemetrySensor) telemetrySensor.textContent = 'NONE';
      if (telemetryThreat) {
        telemetryThreat.textContent = 'ZERO (RESTING)';
        telemetryThreat.className = 't-val good';
      }
      if (telemetryMotor) telemetryMotor.textContent = '0% PWM';
      if (telemetryStatusBadge) {
        telemetryStatusBadge.textContent = 'STANDBY';
        telemetryStatusBadge.className = 'tech-label green';
      }

      addConsoleLog(`RESULT: SUBJECT REMAINS THIRSTY. ATTEMPTS: ${escapeCount}`, 'info');

      currentOledMode = 'HAPPY';
      oledToggles.forEach(b => {
        b.classList.toggle('active', b.dataset.mode === 'HAPPY');
      });
      if (moodLabel) moodLabel.textContent = moodDescriptions['HAPPY'];
    }, 1200);
  }

  // Handle cursor/touch tracking in bench arena
  if (benchArena && benchBot) {
    botX = benchArena.clientWidth / 2;
    botY = benchArena.clientHeight / 2;
    benchBot.style.left = `${botX}px`;
    benchBot.style.top = `${botY}px`;

    const handleBenchMove = (clientX, clientY) => {
      const rect = benchArena.getBoundingClientRect();
      const handX = clientX - rect.left;
      const handY = clientY - rect.top;

      if (benchHand) {
        benchHand.style.left = `${handX}px`;
        benchHand.style.top = `${handY}px`;
      }

      const dx = botX - handX;
      const dy = botY - handY;
      const distance = Math.sqrt(dx * dx + dy * dy);

      // Update distance telemetry in mm
      if (telemetryDist) {
        telemetryDist.textContent = `${Math.round(distance * 1.5)} mm`;
      }

      // Threat proximity threshold (115px)
      if (distance < 115) {
        let sensor = 'IR-01 [FRONT]';
        if (dx > 25) sensor = 'IR-02 [FRONT-L]';
        else if (dx < -25) sensor = 'IR-03 [FRONT-R]';
        triggerLabEvade(handX, handY, sensor);
      }
    };

    benchArena.addEventListener('mousemove', (e) => {
      handleBenchMove(e.clientX, e.clientY);
    });

    benchArena.addEventListener('touchmove', (e) => {
      if (e.touches.length > 0) {
        handleBenchMove(e.touches[0].clientX, e.touches[0].clientY);
      }
    }, { passive: true });

    benchArena.addEventListener('click', (e) => {
      const rect = benchArena.getBoundingClientRect();
      triggerLabEvade(e.clientX - rect.left, e.clientY - rect.top, 'IR-01 [TOUCH]');
    });
  }

  // Reset button in experiment bench
  const btnResetBench = document.getElementById('btnResetBench');
  if (btnResetBench && benchArena && benchBot) {
    btnResetBench.addEventListener('click', () => {
      botX = benchArena.clientWidth / 2;
      botY = benchArena.clientHeight / 2;
      benchBot.style.left = `${botX}px`;
      benchBot.style.top = `${botY}px`;
      escapeCount = 0;
      if (telemetryEscapes) telemetryEscapes.textContent = '0';
      showSpeech("കോർഡിനേറ്റുകൾ റീസെറ്റ് ചെയ്തു!", botX, botY - 60);
      SoundFX.playTrollBoing();
      addConsoleLog('[SYS] Manual coordinate reset executed.', 'warn');
    });
  }


  // ============================================================
  // THREAT DETECTION RADAR CONSOLE INTERACTION
  // ============================================================
  const sensorBtns = document.querySelectorAll('.tactical-sensor-btn, .sensor-node-btn');
  const tacticalVectorItems = document.querySelectorAll('.tactical-vector-item, .threat-row');
  const activeTacticalFeedback = document.getElementById('activeTacticalFeedback') || document.getElementById('activeThreatFeedback');

  const sensorDescriptions = {
    'IR1': 'IR-01 FRONT ASSAULT (Straight Reverse Flee)',
    'IR2': 'IR-02 FRONT-LEFT INVASION (Angled Reverse Drift RIGHT)',
    'IR3': 'IR-03 FRONT-RIGHT INVASION (Angled Reverse Drift LEFT)',
    'IR4': 'IR-04 LEFT LATERAL SNEAK (Clockwise 90° Tank Pivot)',
    'IR5': 'IR-05 RIGHT LATERAL SNEAK (Counter-Clockwise 90° Tank Pivot)'
  };

  sensorBtns.forEach(btn => {
    btn.addEventListener('click', () => {
      const sensor = btn.dataset.sensor;

      sensorBtns.forEach(b => b.classList.remove('active'));
      btn.classList.add('active');

      tacticalVectorItems.forEach(item => {
        item.classList.toggle('active', item.id === `vec-${sensor}` || item.id === `row-${sensor}`);
      });

      if (activeTacticalFeedback) {
        activeTacticalFeedback.textContent = `> ACTIVE VECTOR: ${sensorDescriptions[sensor] || sensor}`;
      }

      // Simulate threat in Experiment arena
      let targetX = botX;
      let targetY = botY;

      if (sensor === 'IR1') { targetY = botY - 70; }
      else if (sensor === 'IR2') { targetX = botX - 50; targetY = botY - 50; }
      else if (sensor === 'IR3') { targetX = botX + 50; targetY = botY - 50; }
      else if (sensor === 'IR4') { targetX = botX - 70; }
      else if (sensor === 'IR5') { targetX = botX + 70; }

      triggerLabEvade(targetX, targetY, `RADAR_TRIGGER_${sensor}`);
    });
  });

  // ============================================================
  // AUDIO EVIDENCE ARCHIVE BUTTONS
  // ============================================================
  const audioPlayTriggers = document.querySelectorAll('.audio-play-trigger, .channel-btn');
  audioPlayTriggers.forEach(btn => {
    btn.addEventListener('click', () => {
      const soundType = btn.dataset.sound;
      const dialogue = btn.dataset.dialogue || '';

      switch (soundType) {
        case 'screech':
          SoundFX.playEvasionScreech();
          break;
        case 'honk':
          SoundFX.playSayippHonk();
          break;
        case 'boing':
          SoundFX.playTrollBoing();
          break;
        case 'siren':
          SoundFX.playAlarmSiren();
          break;
        default:
          SoundFX.playTrollBoing();
          break;
      }

      if (dialogue) {
        speakMemeDialogue(dialogue);
      }

      const originalText = btn.textContent;
      btn.style.background = 'var(--tech-amber)';
      btn.style.color = '#000';
      btn.textContent = '[ PLAYING... ]';
      setTimeout(() => {
        btn.style.background = '';
        btn.style.color = '';
        btn.textContent = originalText;
      }, 700);
    });
  });

  // ============================================================
  // EASTER EGG: DENIAL PROTOCOL
  // ============================================================
  const btnDeny = document.getElementById('btnDenyEverything');
  const denialFeedback = document.getElementById('denialFeedback');
  if (btnDeny && denialFeedback) {
    btnDeny.addEventListener('click', () => {
      SoundFX.playTrollBoing();
      btnDeny.style.background = 'var(--tech-red)';
      btnDeny.textContent = '[ DENIAL REGISTERED ]';
      denialFeedback.style.display = 'block';
      setTimeout(() => {
        btnDeny.disabled = true;
        btnDeny.style.opacity = '0.6';
      }, 500);
    });
  }

});
