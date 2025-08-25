#include "web_ui.h"

const char ESP32FW_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>)rawliteral" WEB_UI_TITLE R"rawliteral(</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: )rawliteral" WEB_UI_BACKGROUND_COLOR R"rawliteral(;
            min-height: 100vh;
            display: flex;
            align-items: center;
            justify-content: center;
            padding: 20px;
        }
        
        .container {
            background: )rawliteral" WEB_UI_CONTAINER_BACKGROUND_COLOR R"rawliteral(;
            border-radius: 20px;
            box-shadow: 0 20px 40px rgba(0,0,0,0.1);
            padding: 40px;
            max-width: 500px;
            width: 100%;
            text-align: center;
        }
        
        .logo {
            font-size: 2.5em;
            font-weight: bold;
            color: )rawliteral" WEB_UI_LOGO_COLOR R"rawliteral(;
            margin-bottom: 10px;
        }
        
        .subtitle {
            color: )rawliteral" WEB_UI_TEXT_COLOR R"rawliteral(;
            margin-bottom: 30px;
            font-size: 1.1em;
        }
        
        .upload-area {
            border: 3px dashed #ddd;
            border-radius: 15px;
            padding: 40px 20px;
            margin: 30px 0;
            transition: all 0.3s ease;
            cursor: pointer;
            position: relative;
        }
        
        .upload-area:hover {
            border-color: )rawliteral" WEB_UI_PRIMARY_BUTTON_COLOR R"rawliteral(;
            background-color: )rawliteral" WEB_UI_UPLOAD_AREA_HOVER_COLOR R"rawliteral(;
        }
        
        .upload-area.dragover {
            border-color: )rawliteral" WEB_UI_PRIMARY_BUTTON_COLOR R"rawliteral(;
            background-color: )rawliteral" WEB_UI_UPLOAD_AREA_DRAGOVER_COLOR R"rawliteral(;
            transform: scale(1.02);
        }
        
        .upload-icon {
            font-size: 3em;
            color: #ddd;
            margin-bottom: 15px;
        }
        
        .upload-text {
            color: )rawliteral" WEB_UI_TEXT_COLOR R"rawliteral(;
            font-size: 1.1em;
            margin-bottom: 15px;
        }
        
        .file-input {
            display: none;
        }
        
        .btn {
            background: )rawliteral" WEB_UI_PRIMARY_BUTTON_COLOR R"rawliteral(;
            color: )rawliteral" WEB_UI_PRIMARY_BUTTON_TEXT_COLOR R"rawliteral(;
            border: none;
            padding: 15px 30px;
            border-radius: 25px;
            font-size: 1.1em;
            cursor: pointer;
            transition: all 0.3s ease;
            margin: 10px;
            min-width: 120px;
        }
        
        .btn:hover {
            transform: translateY(-2px);
            box-shadow: 0 10px 20px rgba(102, 126, 234, 0.3);
        }
        
        .btn:disabled {
            opacity: 0.6;
            cursor: not-allowed;
            transform: none;
        }
        
        .progress-container {
            margin: 20px 0;
            display: none;
        }
        
        .progress-bar {
            width: 100%;
            height: 20px;
            background-color: #f0f0f0;
            border-radius: 10px;
            overflow: hidden;
            margin-bottom: 10px;
        }
        
        .progress-fill {
            height: 100%;
            background: )rawliteral" WEB_UI_PRIMARY_BUTTON_COLOR R"rawliteral(;
            width: 0%;
            transition: width 0.3s ease;
        }
        
        .progress-text {
            color: )rawliteral" WEB_UI_TEXT_COLOR R"rawliteral(;
            font-size: 0.9em;
        }
        
        .status {
            margin: 20px 0;
            padding: 15px;
            border-radius: 10px;
            display: none;
        }
        
        .status.success {
            background-color: #d4edda;
            color: #155724;
            border: 1px solid #c3e6cb;
        }
        
        .status.error {
            background-color: #f8d7da;
            color: #721c24;
            border: 1px solid #f5c6cb;
        }
        
        .mode-selector {
            margin: 20px 0;
        }
        
        .mode-selector label {
            display: inline-block;
            margin: 0 15px;
            cursor: pointer;
            color: )rawliteral" WEB_UI_TEXT_COLOR R"rawliteral(;
        }
        
        .mode-selector input[type="radio"] {
            margin-right: 5px;
        }
        
        .file-info {
            background-color: )rawliteral" WEB_UI_FILE_INFO_BACKGROUND_COLOR R"rawliteral(;
            border-radius: 10px;
            padding: 15px;
            margin: 15px 0;
            display: none;
        }
        
        .file-name {
            font-weight: bold;
            color: )rawliteral" WEB_UI_FILE_INFO_TEXT_COLOR R"rawliteral(;
            margin-bottom: 5px;
        }
        
        .file-size {
            color: )rawliteral" WEB_UI_FILE_INFO_TEXT_COLOR R"rawliteral(;
            font-size: 0.9em;
        }
        
        @media (max-width: 600px) {
            .container {
                padding: 20px;
                margin: 10px;
            }
            
            .logo {
                font-size: 2em;
            }
            
            .upload-area {
                padding: 30px 15px;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="logo">)rawliteral" WEB_UI_LOGO_TEXT R"rawliteral(</div>
        <div class="subtitle">)rawliteral" WEB_UI_SUBTITLE_TEXT R"rawliteral(</div>
        
        <div class="mode-selector">
            <label>
                <input type="radio" name="mode" value="firmware" checked>
                Firmware
            </label>
            <label>
                <input type="radio" name="mode" value="filesystem">
                Filesystem
            </label>
        </div>
        
        <div class="upload-area" id="uploadArea">
            <div class="upload-icon">📁</div>
            <div class="upload-text">Drag & Drop File<br>or<br>Click to Select File</div>
            <input type="file" class="file-input" id="fileInput" accept=".bin">
        </div>
        
        <div class="file-info" id="fileInfo">
            <div class="file-name" id="fileName"></div>
            <div class="file-size" id="fileSize"></div>
        </div>
        
        <button class="btn" id="uploadBtn" disabled>Start Upload</button>
        
        <div class="progress-container" id="progressContainer">
            <div class="progress-bar">
                <div class="progress-fill" id="progressFill"></div>
            </div>
            <div class="progress-text" id="progressText">0%</div>
        </div>
        
        <div class="status" id="status"></div>
    </div>

    <script>
        const uploadArea = document.getElementById('uploadArea');
        const fileInput = document.getElementById('fileInput');
        const uploadBtn = document.getElementById('uploadBtn');
        const progressContainer = document.getElementById('progressContainer');
        const progressFill = document.getElementById('progressFill');
        const progressText = document.getElementById('progressText');
        const status = document.getElementById('status');
        const fileInfo = document.getElementById('fileInfo');
        const fileName = document.getElementById('fileName');
        const fileSize = document.getElementById('fileSize');
        
        let selectedFile = null;
        
        // File selection area click event
        uploadArea.addEventListener('click', () => {
            fileInput.click();
        });
        
        // Drag & drop events
        uploadArea.addEventListener('dragover', (e) => {
            e.preventDefault();
            uploadArea.classList.add('dragover');
        });
        
        uploadArea.addEventListener('dragleave', () => {
            uploadArea.classList.remove('dragover');
        });
        
        uploadArea.addEventListener('drop', (e) => {
            e.preventDefault();
            uploadArea.classList.remove('dragover');
            const files = e.dataTransfer.files;
            if (files.length > 0) {
                handleFileSelect(files[0]);
            }
        });
        
        // File selection event
        fileInput.addEventListener('change', (e) => {
            if (e.target.files.length > 0) {
                handleFileSelect(e.target.files[0]);
            }
        });
        
        // File selection handler
        function handleFileSelect(file) {
            selectedFile = file;
            fileName.textContent = file.name;
            fileSize.textContent = formatFileSize(file.size);
            fileInfo.style.display = 'block';
            uploadBtn.disabled = false;
            hideStatus();
        }
        
        // File size formatting
        function formatFileSize(bytes) {
            if (bytes === 0) return '0 Bytes';
            const k = 1024;
            const sizes = ['Bytes', 'KB', 'MB', 'GB'];
            const i = Math.floor(Math.log(bytes) / Math.log(k));
            return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i];
        }
        
        // Upload button click event
        uploadBtn.addEventListener('click', () => {
            if (selectedFile) {
                uploadFile(selectedFile);
            }
        });
        
        // File upload handler
        function uploadFile(file) {
            const formData = new FormData();
            formData.append('update', file);
            
            const mode = document.querySelector('input[name="mode"]:checked').value;
            
            uploadBtn.disabled = true;
            progressContainer.style.display = 'block';
            hideStatus();
            
            const xhr = new XMLHttpRequest();
            
            // Upload progress monitoring
            xhr.upload.addEventListener('progress', (e) => {
                if (e.lengthComputable) {
                    const percentComplete = (e.loaded / e.total) * 100;
                    progressFill.style.width = percentComplete + '%';
                    progressText.textContent = Math.round(percentComplete) + '%';
                }
            });
            
            // Upload completion handler
            xhr.addEventListener('load', () => {
                uploadBtn.disabled = false;
                progressContainer.style.display = 'none';
                
                if (xhr.status === 200) {
                    const response = xhr.responseText;
                    if (response === 'OK') {
                        showStatus('Upload completed successfully. Device will restart.', 'success');
                    } else {
                        showStatus('Upload failed: ' + response, 'error');
                    }
                } else {
                    showStatus('Upload error: HTTP ' + xhr.status, 'error');
                }
            });
            
            // Error handling
            xhr.addEventListener('error', () => {
                uploadBtn.disabled = false;
                progressContainer.style.display = 'none';
                showStatus('Network error occurred.', 'error');
            });
            
            // Send request
            xhr.open('POST', '/ota/upload?mode=' + mode);
            xhr.send(formData);
        }
        
        // Show status
        function showStatus(message, type) {
            status.textContent = message;
            status.className = 'status ' + type;
            status.style.display = 'block';
        }
        
        // Hide status
        function hideStatus() {
            status.style.display = 'none';
        }
        
        // Mode change handler
        document.querySelectorAll('input[name="mode"]').forEach(radio => {
            radio.addEventListener('change', () => {
                hideStatus();
            });
        });
    </script>
</body>
</html>
)rawliteral";

