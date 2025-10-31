# 🖼️ LSB Image Steganography in C

A simple Image Steganography Program built in C.
It allows users to hide and retrieve secret text from .bmp images using the Least Significant Bit (LSB) technique, secured with a magic string (password) for verification.

---
## ✨ Features
- 🧩 **Encode Message** – Hide secret text from a .txt file into a 24-bit .bmp image
- 🔐 **Magic String Protection** – Ensures decoding only with the correct password
- 📁 **Decode Message** – Extract the hidden message from the stego image
- ⚙️ **File Validation** – Verifies all input and output files before processing
- 💡 **LSB Algorithm** – Embeds data in pixel bits without noticeable image distortion
- 📊 **Capacity Check** – Confirms image size is sufficient before encoding
- 🧱 **Modular Design** – Source code split into multiple files for clarity and maintenance
- 🧠 **Error Handling** – Detects invalid formats, file issues, or mismatched passwords
- 💾 **Output Generation** – Creates a new stego.bmp file containing the hidden message
- 💻 **Command-Line Interface** – Simple and intuitive text-based interaction

---

## 🔐 Magic String & Encoding Logic
- A magic string (like a password) is encoded first to authenticate the message.
- During decoding, the program checks this string to ensure data integrity.
- Encoding and decoding proceed only when the magic string matches correctly.

# 🧮 How It Works
- Reads the source image (.bmp), secret text file, and magic string.
- Encodes the magic string, secret file size, and data into the LSB of image pixels.
- Generates a new stego image (stego.bmp) containing hidden data.
- During decoding, verifies the magic string and retrieves the hidden message if valid.

# 📂 Project Structure
├── main.c # Entry point, handles mode selection (encode/decode)<br>
├── encode.c # Core encoding logic and file operations<br>
├── encode.h # Header for encode functions and structures<br>
├── decode.c # Core decoding logic for retrieving hidden data<br>
├── decode.h # Header for decode functions and structures<br>
├── common.h # Shared macros, enums, and constants<br>
├── types.h # User-defined data types and status enums<br>
├── test.txt # Example secret message file (input)<br>
└── stego.bmp # Output image generated after encoding<br>
