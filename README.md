# Veyon Chat Plugin

This repository contains a free and open-source chat add-on for Veyon, a classroom management software. This plugin provides real-time, feature-rich communication between the Veyon Master (teacher) and connected clients (students).

## Features

This chat add-on offers a wide range of features to enhance classroom communication:

### Core Features

- **Bidirectional Chat**: Real-time, two-way communication between the Master and individual clients.
- **Global Broadcast**: The Master can send messages to all connected clients at once.
- **F10 Hotkey**: A convenient hotkey (F10) to open and close the chat window on both Master and client machines.
- **Customizable Master Name**: The Master can set a custom name to be displayed in the chat.
- **Client Identification**: Clients are identified by their computer names.
- **Chat Clearing**: The Master can clear the chat history for any client.

### Enhanced Features

- **Timestamps**: All messages are timestamped for clarity.
- **Message Status Indicators**: See the status of your messages (Sent, Delivered, Read).
- **Private Messaging**: The Master can send private messages to individual students.
- **Client Status Indicators**: See when a student is typing or away.
- **Message Priority**: Mark messages as Normal, Urgent, or Announcement.
- **Quick Replies**: The Master can use pre-defined templates for quick responses.
- **Customizable UI**: The chat window size and position can be customized and saved.
- **Sound Notifications**: Get audible alerts for new messages (configurable).
- **Keyboard Shortcuts**: Use keyboard shortcuts for common actions like sending messages.
- **System Tray Integration**: Minimize the chat window to the system tray to keep it running in the background.

## Building from Source

To build the Veyon Chat Plugin from source, you will need the following dependencies:

- A C++17 compatible compiler (e.g., GCC, Clang, MSVC)
- CMake 3.16 or higher
- Qt 5.x (Core, Widgets, Network)
- Veyon 4.9.7 development headers

Once you have the dependencies installed, you can build the plugin using the following commands:

```bash
mkdir build
cd build
cmake ..
make
```

## Installation

After building the plugin, you will have a `veyon-chat-plugin.so` (or `.dll` on Windows) file. To install the plugin, simply copy this file to the Veyon plugins directory on both the Master and client machines.

The default plugin directory is typically located at:

- **Linux**: `/usr/lib/veyon/plugins/`
- **Windows**: `C:\Program Files\Veyon\plugins\`

## Usage

Once the plugin is installed, restart the Veyon Master and Veyon Service on all computers. The chat feature will then be available.

- **Master**: You can open the chat window by pressing F10 or by clicking the "Chat" button in the Veyon Master toolbar. The chat window will show a list of connected clients on the left and the chat history on the right.
- **Client**: Students can open the chat window by pressing F10. They will be able to chat with the Master but not with other students.

## Configuration

The chat plugin can be configured through the Veyon Configurator. You can customize the following settings:

- **Master Name**: Set the name that will be displayed for the Master in the chat.
- **Sound Notifications**: Enable or disable sound notifications for new messages.

## License

This project is licensed under the GNU General Public License v2.0. See the `LICENSE` file for more details.

