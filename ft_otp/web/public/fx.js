// Purely cosmetic effects. No auth/OTP logic lives here (see app.js).
(() => {
  "use strict";

  const reduceMotion = window.matchMedia("(prefers-reduced-motion: reduce)").matches;
  const $ = (id) => document.getElementById(id);
  const hex = (n) =>
    Array.from(crypto.getRandomValues(new Uint8Array(n)), (b) => b.toString(16).padStart(2, "0")).join("");

  /* ---- session id + UTC clock ---- */
  const sid = $("fx-sid");
  if (sid) sid.textContent = hex(4).toUpperCase();

  const clock = $("fx-clock");
  const tick = () => {
    if (clock) clock.textContent = new Date().toISOString().slice(11, 19);
  };
  tick();
  setInterval(tick, 1000);

  /* ---- observe app.js DOM updates (read-only) ---- */
  const status = $("status");
  if (status) {
    const syncStatus = () => {
      const t = status.textContent.trim();
      status.dataset.state = /^Connected/i.test(t) ? "ok" : /^Unable/i.test(t) ? "error" : "pending";
    };
    new MutationObserver(syncStatus).observe(status, { childList: true, characterData: true, subtree: true });
    syncStatus();
  }

  const otp = $("otp");
  if (otp && !reduceMotion) {
    new MutationObserver(() => {
      otp.classList.remove("flash");
      void otp.offsetWidth; // restart animation
      otp.classList.add("flash");
    }).observe(otp, { childList: true, characterData: true, subtree: true });
  }

  /* ---- live trace log (decorative, no real data) ---- */
  const log = $("fx-log");
  const lines = [];
  const events = [
    () => `[ OK ] tick ${Math.floor(Date.now() / 30000)}`,
    () => `[ -- ] window sync ${hex(2)}`,
    () => `[ OK ] session ${hex(3)} alive`,
    () => `[ -- ] entropy pool ${hex(4)}`,
    () => `[ OK ] token rotated`,
    () => `[ -- ] heartbeat ${(Math.random() * 9 + 1).toFixed(2)}ms`,
  ];
  const pushLog = () => {
    if (!log) return;
    lines.push(`${new Date().toISOString().slice(11, 19)}  ${events[Math.floor(Math.random() * events.length)]()}`);
    if (lines.length > 6) lines.shift();
    log.textContent = lines.join("\n");
  };
  for (let i = 0; i < 6; i++) pushLog();
  setInterval(pushLog, 1800);

  /* ---- matrix rain ---- */
  const canvas = $("rain");
  if (!canvas || reduceMotion) return;
  const ctx = canvas.getContext("2d");
  const glyphs = "0123456789ABCDEF<>/\\{}[]#$%&*+=";
  const size = 14;
  let cols = 0;
  let drops = [];

  const resize = () => {
    const dpr = Math.min(window.devicePixelRatio || 1, 2);
    canvas.width = window.innerWidth * dpr;
    canvas.height = window.innerHeight * dpr;
    ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
    cols = Math.ceil(window.innerWidth / size);
    drops = Array.from({ length: cols }, () => Math.random() * -50);
  };
  resize();
  window.addEventListener("resize", resize);

  let last = 0;
  const draw = (t) => {
    requestAnimationFrame(draw);
    if (document.hidden || t - last < 60) return;
    last = t;

    ctx.fillStyle = "rgba(3, 6, 10, 0.12)";
    ctx.fillRect(0, 0, window.innerWidth, window.innerHeight);
    ctx.font = `${size}px ui-monospace, Menlo, Consolas, monospace`;

    for (let i = 0; i < cols; i++) {
      const y = drops[i] * size;
      ctx.fillStyle = Math.random() > 0.96 ? "#39ff88" : "#00e5ff";
      ctx.fillText(glyphs[Math.floor(Math.random() * glyphs.length)], i * size, y);
      if (y > window.innerHeight && Math.random() > 0.975) drops[i] = 0;
      drops[i] += 1;
    }
  };
  requestAnimationFrame(draw);
})();