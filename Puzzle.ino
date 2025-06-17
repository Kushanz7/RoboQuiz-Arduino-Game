#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// IR Sensor pins
const int IR_SENSOR_1 = 18;  // IR sensor to start the puzzle
const int IR_SENSOR_2 = 19;  // IR sensor to end the puzzle

// Variables to track IR sensor states
bool puzzleStarted = false;
bool puzzleEnded = false;
int currentPuzzleIndex = 0;

// WiFi credentials
const char* ssid = "HUAWEI nova 5T";  // WiFi network name
const char* password = "12345678";   // WiFi password

// Web server on port 80
WebServer server(80);

// Puzzle structure definition
struct Puzzle {
  String question;
  String options[3];
  String correctAnswer;
};

// Puzzle database (10 total)
Puzzle puzzles[10] = {
  {"I have cities, but no houses. I have mountains, but no trees. I have water, but no fish. What am I?",
   {"A map", "A dream", "A globe"}, "A map"},
  {"What has keys but can't open locks?",
   {"A piano", "A keyboard", "A map"}, "A piano"},
  {"What can travel around the world while staying in the same corner?",
   {"A stamp", "A compass", "A clock"}, "A stamp"},
  {"What has a heart that doesn't beat?",
   {"An artichoke", "A robot", "A stone"}, "An artichoke"},
  {"I speak without a mouth and hear without ears. What am I?",
   {"An echo", "A ghost", "A shadow"}, "An echo"},
  {"What has to be broken before you can use it?",
   {"An egg", "A code", "A bottle"}, "An egg"},
  {"I'm tall when I'm young, and I'm short when I'm old. What am I?",
   {"A candle", "A pencil", "A match"}, "A candle"},
  {"What gets wetter as it dries?",
   {"A towel", "A sponge", "A cloud"}, "A towel"},
  {"The more of me you take, the more you leave behind. What am I?",
   {"Footsteps", "Time", "Memories"}, "Footsteps"},
  {"What has hands but can't clap?",
   {"A clock", "A statue", "A mannequin"}, "A clock"}
};

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);
  
  // Configure IR sensor pins as inputs
  pinMode(IR_SENSOR_1, INPUT);
  pinMode(IR_SENSOR_2, INPUT);
  
  // Connect to WiFi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Define server routes
  server.on("/", handleRoot);
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/puzzle", HTTP_GET, handlePuzzle);
  server.on("/answer", HTTP_GET, handleAnswer);
  server.on("/reset", HTTP_GET, handleReset);
  server.on("/forcestart", HTTP_GET, handleForceStart);
  server.on("/nextpuzzle", HTTP_GET, handleNextPuzzle);
  
  // Start the server
  server.begin();
  Serial.println("HTTP server started");
  
  // Debug message
  Serial.println("Setup complete");
  Serial.println("Waiting for IR sensor 1 (pin 18) to detect an object...");
}

void loop() {
  server.handleClient();
  
  // Check IR sensor 1 (start puzzle)
  if (!puzzleStarted && digitalRead(IR_SENSOR_1) == LOW) {  // Assuming LOW when object detected
    Serial.println("IR Sensor 1 detected an object. Starting puzzle game!");
    puzzleStarted = true;
    puzzleEnded = false;
    currentPuzzleIndex = 0;
    
    // Add debug message
    Serial.println("Game state updated - puzzleStarted: true, puzzleEnded: false");
    
    delay(500);  // Debounce
  }
  
  // Check IR sensor 2 (end puzzle)
  if (puzzleStarted && !puzzleEnded && digitalRead(IR_SENSOR_2) == LOW) {  // Assuming LOW when object detected
    Serial.println("IR Sensor 2 detected an object. Ending puzzle game!");
    puzzleEnded = true;
    
    // Add debug message
    Serial.println("Game state updated - puzzleEnded: true");
    
    delay(500);  // Debounce
  }
  
  delay(50);  // Small delay to prevent hogging the CPU
}

// void handleRoot() {
//   String html = "<!DOCTYPE html><html><head>";
//   html += "<title>ESP32 Puzzle Game</title>";
//   html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
//   html += "<style>";
//   html += "body { font-family: Arial, sans-serif; text-align: center; margin: 20px; }";
//   html += "h1 { color: #333; }";
//   html += ".container { max-width: 600px; margin: 0 auto; padding: 20px; border: 1px solid #ddd; border-radius: 10px; }";
//   html += ".status { margin: 20px 0; padding: 10px; border-radius: 5px; }";
//   html += ".active { background-color: #d4edda; color: #155724; }";
//   html += ".inactive { background-color: #f8d7da; color: #721c24; }";
//   html += ".waiting { background-color: #fff3cd; color: #856404; }";
//   html += ".puzzle-container { margin-top: 20px; padding: 15px; background-color: #f8f9fa; border-radius: 5px; }";
//   html += ".options { display: flex; flex-direction: column; gap: 10px; margin-top: 15px; }";
//   html += ".option-btn { padding: 10px; border: 1px solid #007bff; background-color: #e7f3ff; border-radius: 5px; cursor: pointer; }";
//   html += ".option-btn:hover { background-color: #cce5ff; }";
//   html += ".result { margin-top: 15px; font-weight: bold; }";
//   html += ".correct { color: #28a745; }";
//   html += ".incorrect { color: #dc3545; }";
//   html += ".hidden { display: none; }";
//   html += "</style>";
//   html += "</head><body>";
//   html += "<div class='container'>";
//   html += "<h1>ESP32 Puzzle Game</h1>";
//   html += "<div id='status-container' class='status waiting'>Checking status...</div>";
//   html += "<button onclick='forcePuzzleStart()' style='margin-bottom:10px;padding:5px;'>Force Puzzle Start</button>";
//   html += "<div id='puzzle-container' class='puzzle-container hidden'>";
//   html += "<h2>Puzzle</h2>";
//   html += "<div id='question'></div>";
//   html += "<div class='options' id='options'></div>";
//   html += "<div id='result' class='result hidden'></div>";
//   html += "<button id='next-btn' class='hidden' onclick='loadNextPuzzle()'>Next Puzzle</button>";
//   html += "</div>";
//   html += "<div id='end-message' class='hidden'><h2>Game Over!</h2><p>Thank you for playing!</p>";
//   html += "<button onclick='resetGame()'>Reset</button></div>";
//   html += "</div>";
  
//   html += "<script>";
//   html += "let currentPuzzle = 0;";
//   html += "let score = 0;";
  
//   html += "function updateStatus() {";
//   html += "  fetch('/status').then(response => response.json()).then(data => {";
//   html += "    console.log('Status data received:', data);";  // Debug logging
//   html += "    const statusContainer = document.getElementById('status-container');";
//   html += "    const puzzleContainer = document.getElementById('puzzle-container');";
//   html += "    const endMessage = document.getElementById('end-message');";
  
//   html += "    if (data.ended) {";
//   html += "      statusContainer.className = 'status inactive';";
//   html += "      statusContainer.textContent = 'Game has ended. Final score: ' + score + '/' + currentPuzzle;";
//   html += "      puzzleContainer.classList.add('hidden');";
//   html += "      endMessage.classList.remove('hidden');";
//   html += "    } else if (data.started) {";
//   html += "      statusContainer.className = 'status active';";
//   html += "      statusContainer.textContent = 'Game is active! Score: ' + score + '/' + currentPuzzle;";
//   html += "      puzzleContainer.classList.remove('hidden');";
//   html += "      loadPuzzle();";
//   html += "    } else {";
//   html += "      statusContainer.className = 'status waiting';";
//   html += "      statusContainer.textContent = 'Waiting for game to start... (IR Sensor 1 needs to detect object)';";
//   html += "      puzzleContainer.classList.add('hidden');";
//   html += "    }";
//   html += "  }).catch(error => {";
//   html += "    console.error('Error fetching status:', error);";
//   html += "  });";
//   html += "}";
  
//   html += "function loadPuzzle() {";
//   html += "  fetch('/puzzle').then(response => response.json()).then(data => {";
//   html += "    if (data.index !== undefined) {";
//   html += "      currentPuzzle = data.index + 1;";
//   html += "      document.getElementById('question').textContent = data.question;";
//   html += "      const optionsContainer = document.getElementById('options');";
//   html += "      optionsContainer.innerHTML = '';";
//   html += "      data.options.forEach(option => {";
//   html += "        const btn = document.createElement('button');";
//   html += "        btn.className = 'option-btn';";
//   html += "        btn.textContent = option;";
//   html += "        btn.onclick = function() { checkAnswer(option); };";
//   html += "        optionsContainer.appendChild(btn);";
//   html += "      });";
//   html += "      document.getElementById('result').classList.add('hidden');";
//   html += "      document.getElementById('next-btn').classList.add('hidden');";
//   html += "    }";
//   html += "  });";
//   html += "}";
  
//   html += "function checkAnswer(selected) {";
//   html += "  const params = new URLSearchParams();";
//   html += "  params.append('answer', selected);";
//   html += "  fetch('/answer?' + params.toString()).then(response => response.json()).then(data => {";
//   html += "    const resultElement = document.getElementById('result');";
//   html += "    resultElement.classList.remove('hidden');";
//   html += "    if (data.correct) {";
//   html += "      resultElement.className = 'result correct';";
//   html += "      resultElement.textContent = 'Correct! 🎉';";
//   html += "      score++;";
//   html += "    } else {";
//   html += "      resultElement.className = 'result incorrect';";
//   html += "      resultElement.textContent = 'Incorrect. The correct answer is: ' + data.correctAnswer;";
//   html += "    }";
//   html += "    document.getElementById('next-btn').classList.remove('hidden');";
//   html += "    document.querySelectorAll('.option-btn').forEach(btn => {";
//   html += "      btn.disabled = true;";
//   html += "    });";
//   html += "  });";
//   html += "}";
  
//   html += "function loadNextPuzzle() {";
//   html += "  console.log('Loading next puzzle...');";
//   html += "  fetch('/nextpuzzle').then(response => response.json()).then(data => {";
//   html += "    console.log('Next puzzle response:', data);";
//   html += "    if (data.success) {";
//   html += "      document.getElementById('result').classList.add('hidden');";
//   html += "      document.getElementById('next-btn').classList.add('hidden');";
//   html += "      loadPuzzle();";
//   html += "    }";
//   html += "  }).catch(error => {";
//   html += "    console.error('Error loading next puzzle:', error);";
//   html += "  });";
//   html += "}";
  
//   html += "function resetGame() {";
//   html += "  fetch('/reset').then(() => {";
//   html += "    score = 0;";
//   html += "    currentPuzzle = 0;";
//   html += "    updateStatus();";
//   html += "  });";
//   html += "}";
  
//   html += "function forcePuzzleStart() {";
//   html += "  fetch('/forcestart').then(response => response.json()).then(data => {";
//   html += "    console.log('Force start response:', data);";
//   html += "    updateStatus();";
//   html += "  });";
//   html += "}";
  
//   html += "// Check status initially and then every 2 seconds";
//   html += "updateStatus();";
//   html += "setInterval(updateStatus, 2000);";
//   html += "</script>";
  
//   html += "</body></html>";
  
//   server.send(200, "text/html", html);
// }

void handleRoot() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<title>ESP32 Puzzle Game</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; text-align: center; margin: 20px; }";
  html += "h1 { color: #333; }";
  html += ".container { max-width: 600px; margin: 0 auto; padding: 20px; border: 1px solid #ddd; border-radius: 10px; }";
  html += ".status { margin: 20px 0; padding: 10px; border-radius: 5px; }";
  html += ".active { background-color: #d4edda; color: #155724; }";
  html += ".inactive { background-color: #f8d7da; color: #721c24; }";
  html += ".waiting { background-color: #fff3cd; color: #856404; }";
  html += ".puzzle-container { margin-top: 20px; padding: 15px; background-color: #f8f9fa; border-radius: 5px; }";
  html += ".options { display: flex; flex-direction: column; gap: 10px; margin-top: 15px; }";
  html += ".option-btn { padding: 10px; border: 1px solid #007bff; background-color: #e7f3ff; border-radius: 5px; cursor: pointer; }";
  html += ".option-btn:hover { background-color: #cce5ff; }";
  html += ".result { margin-top: 15px; font-weight: bold; }";
  html += ".correct { color: #28a745; }";
  html += ".incorrect { color: #dc3545; }";
  html += ".hidden { display: none; }";
  html += "</style>";
  html += "</head><body>";
  html += "<div class='container'>";
  html += "<h1>ESP32 Puzzle Game</h1>";
  html += "<div id='status-container' class='status waiting'>Checking status...</div>";

  html += "<button onclick='forcePuzzleStart()' style='margin-bottom:10px;padding:5px;'>Force Puzzle Start</button>";
  html += "<div id='puzzle-container' class='puzzle-container hidden'>";
  html += "<h2>Puzzle</h2>";
  html += "<div id='question'></div>";
  html += "<div class='options' id='options'></div>";
  html += "<div id='result' class='result hidden'></div>";
  html += "<button id='next-btn' class='hidden' onclick='loadNextPuzzle()'>Next Puzzle</button>";
  html += "</div>";
  html += "<div id='end-message' class='hidden'><h2>Game Over!</h2><p>Thank you for playing!</p>";
  html += "<button onclick='resetGame()'>Reset</button></div>";
  html += "</div>";

  html += "<script>";
  html += "let currentPuzzle = 0;";
  html += "let score = 0;";

  html += "function updateStatus() {";
  html += "  fetch('/status').then(response => response.json()).then(data => {";
  html += "    console.log('Status data received:', data);";
  html += "    const statusContainer = document.getElementById('status-container');";
  html += "    const puzzleContainer = document.getElementById('puzzle-container');";
  html += "    const endMessage = document.getElementById('end-message');";
  html += "    const nextBtn = document.getElementById('next-btn');";

  html += "    if (data.ended) {";
  html += "      statusContainer.className = 'status inactive';";
  html += "      statusContainer.textContent = 'Game has ended. Final score: ' + score + '/' + (data.currentIndex + 1);";
  html += "      puzzleContainer.classList.add('hidden');";
  html += "      endMessage.classList.remove('hidden');";
  html += "      nextBtn.classList.add('hidden');";
  html += "      document.querySelectorAll('.option-btn').forEach(btn => { btn.disabled = true; });";
  html += "    } else if (data.started) {";
  html += "      statusContainer.className = 'status active';";
  html += "      statusContainer.textContent = 'Game is active! Score: ' + score + '/' + (data.currentIndex + 1);";
  html += "      puzzleContainer.classList.remove('hidden');";
  html += "      endMessage.classList.add('hidden');";
  html += "      if (data.currentIndex !== (currentPuzzle - 1) || !document.getElementById('question').textContent) {";
  html += "         currentPuzzle = data.currentIndex + 1;";
  html += "         loadPuzzle();";
  html += "      }";
  html += "    } else {";
  html += "      statusContainer.className = 'status waiting';";
  html += "      statusContainer.textContent = 'Waiting for game to start... (IR Sensor 1 needs to detect object)';";
  html += "      puzzleContainer.classList.add('hidden');";
  html += "      endMessage.classList.add('hidden');";
  html += "    }";
  html += "  }).catch(error => {";
  html += "    console.error('Error fetching status:', error);";
  html += "  });";
  html += "}";

  html += "function loadPuzzle() {";
  html += "  fetch('/status').then(response => response.json()).then(statusData => {";
  html += "    if (statusData.ended) {";
  html += "      console.log('Game has ended, preventing puzzle load.');";
  html += "      updateStatus();";
  html += "      return;";
  html += "    }";
  html += "    fetch('/puzzle').then(response => response.json()).then(data => {";
  html += "      if (data.index !== undefined) {";
  html += "        currentPuzzle = data.index + 1;";
  html += "        document.getElementById('question').textContent = data.question;";
  html += "        const optionsContainer = document.getElementById('options');";
  html += "        optionsContainer.innerHTML = '';";
  html += "        data.options.forEach(option => {";
  html += "          const btn = document.createElement('button');";
  html += "          btn.className = 'option-btn';";
  html += "          btn.textContent = option;";
  html += "          btn.onclick = function() { checkAnswer(option); };";
  html += "          optionsContainer.appendChild(btn);";
  html += "        });";
  html += "        document.getElementById('result').classList.add('hidden');";
  html += "        document.getElementById('next-btn').classList.add('hidden');";
  html += "        document.querySelectorAll('.option-btn').forEach(btn => {";
  html += "          btn.disabled = false; ";
  html += "        });";
  html += "        updateStatus();";
  html += "      }";
  html += "    });";
  html += "  }).catch(error => {";
  html += "    console.error('Error fetching status for puzzle load check:', error);";
  html += "  });";
  html += "}";

  html += "function checkAnswer(selected) {";
  html += "  fetch('/status').then(response => response.json()).then(statusData => {";
  html += "    if (statusData.ended) {";
  html += "      console.log('Game has ended, preventing answer submission.');";
  html += "      updateStatus();";
  html += "      return;";
  html += "    }";
  html += "    const params = new URLSearchParams();";
  html += "    params.append('answer', selected);";
  html += "    fetch('/answer?' + params.toString()).then(response => response.json()).then(data => {";
  html += "      const resultElement = document.getElementById('result');";
  html += "      resultElement.classList.remove('hidden');";
  html += "      if (data.correct) {";
  html += "        resultElement.className = 'result correct';";
  html += "        resultElement.textContent = 'Correct! 🎉';";
  html += "        score++;";
  html += "      } else {";
  html += "        resultElement.className = 'result incorrect';";
  html += "        resultElement.textContent = 'Incorrect. The correct answer is: ' + data.correctAnswer;";
  html += "      }";
  html += "      document.getElementById('next-btn').classList.remove('hidden');";
  html += "      document.querySelectorAll('.option-btn').forEach(btn => {";
  html += "        btn.disabled = true;";
  html += "      });";
  html += "      updateStatus();";
  html += "    });";
  html += "  }).catch(error => {";
  html += "    console.error('Error fetching status for answer check:', error);";
  html += "  });";
  html += "}";

  html += "function loadNextPuzzle() {";
  html += "  fetch('/status').then(response => response.json()).then(statusData => {";
  html += "    if (statusData.ended) {";
  html += "      console.log('Game has ended, preventing next puzzle load.');";
  html += "      updateStatus();";
  html += "      return;";
  html += "    }";
  html += "    console.log('Loading next puzzle...');";
  html += "    fetch('/nextpuzzle').then(response => response.json()).then(data => {";
  html += "      console.log('Next puzzle response:', data);";
  html += "      if (data.success) {";
  html += "        document.getElementById('result').classList.add('hidden');";
  html += "        document.getElementById('next-btn').classList.add('hidden');";
  html += "        loadPuzzle();";
  html += "      } else {";
  html += "        console.warn('Could not load next puzzle:', data.message);";
  html += "        updateStatus();";
  html += "      }";
  html += "    }).catch(error => {";
  html += "      console.error('Error loading next puzzle:', error);";
  html += "    });";
  html += "  }).catch(error => {";
  html += "    console.error('Error fetching status for next puzzle check:', error);";
  html += "  });";
  html += "}";

  html += "function resetGame() {";
  html += "  fetch('/reset').then(() => {";
  html += "    score = 0;";
  html += "    currentPuzzle = 0;";
  html += "    updateStatus();";
  html += "  });";
  html += "}";

  html += "function forcePuzzleStart() {";
  html += "  fetch('/forcestart').then(response => response.json()).then(data => {";
  html += "    console.log('Force start response:', data);";
  html += "    score = 0;";
  html += "    currentPuzzle = 0;";
  html += "    updateStatus();";
  html += "  });";
  html += "}";

  html += "// Check status initially and then every 2 seconds";
  html += "updateStatus();";
  html += "setInterval(updateStatus, 2000);";
  html += "</script>";

  html += "</body></html>";

  server.send(200, "text/html", html);
}

// void handleRoot() {
//   String html = "<!DOCTYPE html><html><head>";
//   html += "<title>ESP32 Puzzle Game</title>";
//   html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
//   html += "<style>";
//   html += "body { font-family: Arial, sans-serif; text-align: center; margin: 20px; }";
//   html += "h1 { color: #333; }";
//   html += ".container { max-width: 600px; margin: 0 auto; padding: 20px; border: 1px solid #ddd; border-radius: 10px; }";
//   html += ".status { margin: 20px 0; padding: 10px; border-radius: 5px; }";
//   html += ".active { background-color: #d4edda; color: #155724; }";
//   html += ".inactive { background-color: #f8d7da; color: #721c24; }";
//   html += ".waiting { background-color: #fff3cd; color: #856404; }";
//   html += ".puzzle-container { margin-top: 20px; padding: 15px; background-color: #f8f9fa; border-radius: 5px; }";
//   html += ".options { display: flex; flex-direction: column; gap: 10px; margin-top: 15px; }";
//   html += ".option-btn { padding: 10px; border: 1px solid #007bff; background-color: #e7f3ff; border-radius: 5px; cursor: pointer; }";
//   html += ".option-btn:hover { background-color: #cce5ff; }";
//   html += ".result { margin-top: 15px; font-weight: bold; }";
//   html += ".correct { color: #28a745; }";
//   html += ".incorrect { color: #dc3545; }";
//   html += ".hidden { display: none; }";
//   html += "</style>";
//   html += "</head><body>";
//   html += "<div class='container'>";
//   html += "<h1>ESP32 Puzzle Game</h1>";
//   html += "<div id='status-container' class='status waiting'>Checking status...</div>";

//   // --- HTML BUTTONS AND ELEMENTS COME FIRST ---
//   html += "<button onclick='forcePuzzleStart()' style='margin-bottom:10px;padding:5px;'>Force Puzzle Start</button>";
//   html += "<div id='puzzle-container' class='puzzle-container hidden'>";
//   html += "<h2>Puzzle</h2>";
//   html += "<div id='question'></div>";
//   html += "<div class='options' id='options'></div>";
//   html += "<div id='result' class='result hidden'></div>";
//   html += "<button id='next-btn' class='hidden' onclick='loadNextPuzzle()'>Next Puzzle</button>";
//   html += "</div>";
//   html += "<div id='end-message' class='hidden'><h2>Game Over!</h2><p>Thank you for playing!</p>";
//   html += "<button onclick='resetGame()'>Reset</button></div>";
//   html += "</div>";

//   // --- THEN, YOUR ENTIRE JAVASCRIPT BLOCK ---
//   html += "<script>";
//   html += "let currentPuzzle = 0;";
//   html += "let score = 0;";
//   html += "let gameIsOver = false;"; // This variable is crucial for game flow control

//   html += "function updateStatus() {";
//   html += "  fetch('/status').then(response => response.json()).then(data => {";
//   html += "    console.log('Status data received:', data);";
//   html += "    const statusContainer = document.getElementById('status-container');";
//   html += "    const puzzleContainer = document.getElementById('puzzle-container');";
//   html += "    const endMessage = document.getElementById('end-message');";
//   html += "    const nextBtn = document.getElementById('next-btn');"; // Get next button element

//   html += "    gameIsOver = data.ended; // Update game state based on server response";

//   html += "    if (gameIsOver) {";
//   html += "      statusContainer.className = 'status inactive';";
//   html += "      statusContainer.textContent = 'Game has ended. Final score: ' + score + '/' + (data.currentIndex + 1);"; // Accurate final score
//   html += "      puzzleContainer.classList.add('hidden');";
//   html += "      endMessage.classList.remove('hidden');";
//   html += "      nextBtn.classList.add('hidden'); // Hide next button at game end";
//   html += "    } else if (data.started) {";
//   html += "      statusContainer.className = 'status active';";
//   html += "      statusContainer.textContent = 'Game is active! Score: ' + score + '/' + (data.currentIndex + 1);"; // Accurate current score
//   html += "      puzzleContainer.classList.remove('hidden');";
//   html += "      endMessage.classList.add('hidden');";
//   html += "      // Only load puzzle if it's the first time or if the index has changed";
//   html += "      if (data.currentIndex !== (currentPuzzle - 1) || !document.getElementById('question').textContent) {";
//   html += "         currentPuzzle = data.currentIndex + 1; // Sync client current puzzle index with server";
//   html += "         loadPuzzle();";
//   html += "      }";
//   html += "    } else {";
//   html += "      statusContainer.className = 'status waiting';";
//   html += "      statusContainer.textContent = 'Waiting for game to start... (IR Sensor 1 needs to detect object)';";
//   html += "      puzzleContainer.classList.add('hidden');";
//   html += "      endMessage.classList.add('hidden');";
//   html += "    }";
//   html += "  }).catch(error => {";
//   html += "    console.error('Error fetching status:', error);";
//   html += "  });";
//   html += "}";

//   html += "function loadPuzzle() {";
//   html += "  if (gameIsOver) return; // Prevent loading new puzzles if game is over";
//   html += "  fetch('/puzzle').then(response => response.json()).then(data => {";
//   html += "    if (data.index !== undefined) {";
//   html += "      currentPuzzle = data.index + 1;";
//   html += "      document.getElementById('question').textContent = data.question;";
//   html += "      const optionsContainer = document.getElementById('options');";
//   html += "      optionsContainer.innerHTML = '';";
//   html += "      data.options.forEach(option => {";
//   html += "        const btn = document.createElement('button');";
//   html += "        btn.className = 'option-btn';";
//   html += "        btn.textContent = option;";
//   html += "        btn.onclick = function() { checkAnswer(option); };";
//   html += "        optionsContainer.appendChild(btn);";
//   html += "      });";
//   html += "      document.getElementById('result').classList.add('hidden');";
//   html += "      document.getElementById('next-btn').classList.add('hidden');";
//   html += "      document.querySelectorAll('.option-btn').forEach(btn => {";
//   html += "        btn.disabled = false; // Re-enable buttons for new puzzle";
//   html += "      });";
//   html += "      updateStatus(); // Refresh the score display after loading a new puzzle";
//   html += "    }";
//   html += "  });";
//   html += "}";

//   html += "function checkAnswer(selected) {";
//   html += "  if (gameIsOver) return; // Prevent answering if game is over";
//   html += "  const params = new URLSearchParams();";
//   html += "  params.append('answer', selected);";
//   html += "  fetch('/answer?' + params.toString()).then(response => response.json()).then(data => {";
//   html += "    const resultElement = document.getElementById('result');";
//   html += "    resultElement.classList.remove('hidden');";
//   html += "    if (data.correct) {";
//   html += "      resultElement.className = 'result correct';";
//   html += "      resultElement.textContent = 'Correct! 🎉';";
//   html += "      score++;";
//   html += "    } else {";
//   html += "      resultElement.className = 'result incorrect';";
//   html += "      resultElement.textContent = 'Incorrect. The correct answer is: ' + data.correctAnswer;";
//   html += "    }";
//   html += "    document.getElementById('next-btn').classList.remove('hidden');";
//   html += "    document.querySelectorAll('.option-btn').forEach(btn => {";
//   html += "      btn.disabled = true;";
//   html += "    });";
//   html += "    updateStatus(); // Refresh the score display after checking an answer";
//   html += "  });";
//   html += "}";

//   html += "function loadNextPuzzle() {";
//   html += "  if (gameIsOver) return; // Prevent loading next puzzle if game is over";
//   html += "  console.log('Loading next puzzle...');";
//   html += "  fetch('/nextpuzzle').then(response => response.json()).then(data => {";
//   html += "    console.log('Next puzzle response:', data);";
//   html += "    if (data.success) {";
//   html += "      document.getElementById('result').classList.add('hidden');";
//   html += "      document.getElementById('next-btn').classList.add('hidden');";
//   html += "      loadPuzzle();";
//   html += "    } else {";
//   html += "      console.warn('Could not load next puzzle:', data.message);";
//   html += "      updateStatus(); // Force status update to reflect game end if server says it's over";
//   html += "    }";
//   html += "  }).catch(error => {";
//   html += "    console.error('Error loading next puzzle:', error);";
//   html += "  });";
//   html += "}";

//   html += "function resetGame() {";
//   html += "  fetch('/reset').then(() => {";
//   html += "    score = 0;";
//   html += "    currentPuzzle = 0;";
//   html += "    gameIsOver = false; // Reset game state on client";
//   html += "    updateStatus();";
//   html += "  });";
//   html += "}";

//   html += "function forcePuzzleStart() {";
//   html += "  fetch('/forcestart').then(response => response.json()).then(data => {";
//   html += "    console.log('Force start response:', data);";
//   html += "    gameIsOver = false; // Reset game state on client";
//   html += "    score = 0; // Reset score on force start";
//   html += "    currentPuzzle = 0; // Reset puzzle index on force start";
//   html += "    updateStatus();";
//   html += "  });";
//   html += "}";

//   html += "// Check status initially and then every 2 seconds";
//   html += "updateStatus();";
//   html += "setInterval(updateStatus, 2000);";
//   html += "</script>";

//   html += "</body></html>";

//   server.send(200, "text/html", html);
// }

void handleStatus() {
  String json = "{\"started\":" + String(puzzleStarted ? "true" : "false") + ",";
  json += "\"ended\":" + String(puzzleEnded ? "true" : "false") + ",";
  json += "\"currentIndex\":" + String(currentPuzzleIndex) + "}";
  server.send(200, "application/json", json);
  
  // Debug output to serial monitor
  Serial.print("Status request received. Puzzle started: ");
  Serial.print(puzzleStarted ? "Yes" : "No");
  Serial.print(", Puzzle ended: ");
  Serial.print(puzzleEnded ? "Yes" : "No");
  Serial.print(", Current puzzle index: ");
  Serial.println(currentPuzzleIndex);
}

void handlePuzzle() {
  if (currentPuzzleIndex >= 10) {
    currentPuzzleIndex = 0;  // Reset to first puzzle if we've gone through all
  }
  
  Puzzle currentPuzzle = puzzles[currentPuzzleIndex];
  
  String json = "{\"index\":" + String(currentPuzzleIndex) + ",";
  json += "\"question\":\"" + currentPuzzle.question + "\",";
  json += "\"options\":[";
  for (int i = 0; i < 3; i++) {
    json += "\"" + currentPuzzle.options[i] + "\"";
    if (i < 2) json += ",";
  }
  json += "]}";
  
  server.send(200, "application/json", json);
}

void handleAnswer() {
  String selectedAnswer = server.arg("answer");
  Puzzle currentPuzzle = puzzles[currentPuzzleIndex];
  bool isCorrect = (selectedAnswer == currentPuzzle.correctAnswer);
  
  String json = "{\"correct\":" + String(isCorrect ? "true" : "false") + ",";
  json += "\"correctAnswer\":\"" + currentPuzzle.correctAnswer + "\"}";
  
  server.send(200, "application/json", json);
  
  // Move to next puzzle after answering
  // We'll now let the "Next" button handle this instead of doing it automatically
  // currentPuzzleIndex is now advanced by the /nextpuzzle endpoint
}

void handleReset() {
  puzzleStarted = false;
  puzzleEnded = false;
  currentPuzzleIndex = 0;
  server.send(200, "application/json", "{\"reset\":true}");
  Serial.println("Game reset by web request");
}

void handleForceStart() {
  puzzleStarted = true;
  puzzleEnded = false;
  currentPuzzleIndex = 0;
  server.send(200, "application/json", "{\"forceStart\":true}");
  Serial.println("Game force started by web request");
}

void handleNextPuzzle() {
  // Only allow moving to the next puzzle if the game hasn't ended
  if (!puzzleEnded) {
    if (currentPuzzleIndex < 9) {
      currentPuzzleIndex++;
    } else {
      // Option 1: Loop back to the first puzzle (current behavior)
      currentPuzzleIndex = 0;
      // Option 2: Consider the game truly ended if all puzzles are completed
      // puzzleEnded = true; // Uncomment this if you want it to end after all puzzles
    }

    Serial.print("Moving to next puzzle. New index: ");
    Serial.println(currentPuzzleIndex);
    server.send(200, "application/json", "{\"success\":true,\"newIndex\":" + String(currentPuzzleIndex) + "}");
  } else {
    // If the puzzle has ended, send a response indicating it's not possible to move
    // REMOVE THE REDUNDANT FOURTH ARGUMENT HERE
    server.send(200, "application/json", "{\"success\":false,\"message\":\"Puzzle has ended.\"}"); // Corrected line
    Serial.println("Attempted to move to next puzzle, but game has ended.");
  }
}