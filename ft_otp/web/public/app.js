const otpElement = document.getElementById("otp");
const statusElement = document.getElementById("status");
const countdownElement = document.getElementById("countdown");
const timerBar = document.getElementById("timer-bar");

const qrImage = document.getElementById("qr-code");
const qrStatus = document.getElementById("qr-status");

const refreshButton = document.getElementById("refresh-button");
const qrButton = document.getElementById("qr-button");

let lastTimeStep = -1;
let requestInProgress = false;

async function loadOtp() {
  if (requestInProgress) return;

  requestInProgress = true;
  refreshButton.disabled = true;

  try {
    const response = await fetch("/api/otp", {
      cache: "no-store"
    });

    if (!response.ok) {
      throw new Error("OTP request failed");
    }

    const data = await response.json();

    if (!/^\d{6}$/.test(data.otp)) {
      throw new Error("Invalid OTP response");
    }

    otpElement.textContent = data.otp;
    statusElement.textContent = "Connected";
  } catch {
    statusElement.textContent = "Unable to load OTP";
    otpElement.textContent = "------";
  } finally {
    requestInProgress = false;
    refreshButton.disabled = false;
  }
}

async function loadQrCode() {
  qrButton.disabled = true;
  qrStatus.textContent = "Loading QR code...";
  qrImage.hidden = true;

  try {
    const response = await fetch("/api/qr-code", {
      cache: "no-store"
    });

    if (!response.ok) {
      throw new Error("QR request failed");
    }

    const data = await response.json();

    if (
      typeof data.qrCode !== "string" ||
      !data.qrCode.startsWith("data:image/png;base64,")
    ) {
      throw new Error("Invalid QR response");
    }

    qrImage.src = data.qrCode;
    qrImage.hidden = false;
    qrStatus.textContent = "";
  } catch {
    qrStatus.textContent = "Unable to load QR code.";
  } finally {
    qrButton.disabled = false;
  }
}

function updateCountdown() {
  const now = Date.now();
  const timeStep = Math.floor(now / 30000);
  const secondsRemaining = 30 - Math.floor((now % 30000) / 1000);

  countdownElement.textContent = String(secondsRemaining);
  timerBar.style.width =
    `${((30 - secondsRemaining) / 30) * 100}%`;

  if (timeStep !== lastTimeStep) {
    lastTimeStep = timeStep;
    void loadOtp();
  }
}

refreshButton.addEventListener("click", () => {
  void loadOtp();
});

qrButton.addEventListener("click", () => {
  void loadQrCode();
});

updateCountdown();
void loadQrCode();

setInterval(updateCountdown, 250);