# Veyon Chat Plugin - Windows Installation Guide

**Author**: Manus AI  
**Date**: 2025-09-30  
**Target Platform**: Windows 10/11 with Veyon 4.9.7

## Prerequisites

Before installing the Veyon Chat Plugin, ensure you have the following components installed on your Windows system.

### Required Software

**Veyon 4.9.7**: The chat plugin is specifically designed for Veyon version 4.9.7. You can download Veyon from the official website at [https://veyon.io](https://veyon.io). Ensure that both the Veyon Master and Veyon Service are properly installed and configured on your network.

**Microsoft Visual C++ Redistributable**: The plugin requires the Visual C++ Redistributable for Visual Studio 2019 or later. This is typically installed automatically with Veyon, but if you encounter missing DLL errors, download and install it from the Microsoft website.

### Development Prerequisites (If Building from Source)

If you plan to build the plugin from source code rather than using a pre-compiled binary, you will need additional development tools.

**Visual Studio 2019 or Later**: Install Visual Studio Community (free) or Professional with the "Desktop development with C++" workload. This includes the MSVC compiler and Windows SDK components required for building the plugin.

**Qt 5.15 or Later**: Download and install Qt from [https://www.qt.io](https://www.qt.io). During installation, ensure you select the MSVC 2019 64-bit component that matches your Visual Studio installation. The plugin requires Qt Core, Qt Widgets, and Qt Network modules.

**CMake 3.16 or Higher**: Download CMake from [https://cmake.org](https://cmake.org) and install it with the option to add CMake to the system PATH. This build system is required to generate Visual Studio project files from the plugin source code.

**Git for Windows**: Install Git from [https://git-scm.com](https://git-scm.com) to clone the plugin repository and manage source code versions.

## Installation Methods

You can install the Veyon Chat Plugin using either a pre-compiled binary (recommended for most users) or by building from source code (for developers or advanced users).

### Method 1: Pre-compiled Binary Installation

This method is recommended for most users as it requires no development tools or compilation steps.

**Step 1: Download the Plugin Binary**  
Download the pre-compiled `veyon-chat-plugin.dll` file from the plugin releases page. Ensure you download the version that matches your system architecture (x64 for 64-bit Windows, which is standard for modern Veyon installations).

**Step 2: Locate Veyon Plugin Directory**  
Navigate to your Veyon installation directory. The default location is typically `C:\Program Files\Veyon\` or `C:\Program Files (x86)\Veyon\` depending on your installation. Within this directory, locate the `plugins` subdirectory. If the plugins directory does not exist, create it manually.

**Step 3: Install the Plugin**  
Copy the `veyon-chat-plugin.dll` file to the Veyon plugins directory. You will need administrator privileges to copy files to the Program Files directory. Right-click on File Explorer and select "Run as administrator" or use an elevated command prompt.

**Step 4: Restart Veyon Services**  
Restart both the Veyon Master application and the Veyon Service on all computers where you installed the plugin. On the master computer, simply close and reopen the Veyon Master application. On client computers, restart the Veyon Service through the Windows Services management console or by running the following commands in an elevated command prompt:

```cmd
net stop VeyonService
net start VeyonService
```

### Method 2: Building from Source

This method is for users who want to compile the plugin themselves or make modifications to the source code.

**Step 1: Clone the Repository**  
Open a command prompt or Git Bash and clone the plugin repository to your local machine:

```cmd
git clone https://github.com/your-repo/veyon-chat-plugin.git
cd veyon-chat-plugin
```

**Step 2: Configure Build Environment**  
Ensure that Qt, Visual Studio, and CMake are properly installed and configured. Add Qt's bin directory to your system PATH environment variable. The typical path is `C:\Qt\5.15.2\msvc2019_64\bin`.

**Step 3: Generate Visual Studio Project**  
Create a build directory and use CMake to generate Visual Studio project files:

```cmd
mkdir build
cd build
cmake -G "Visual Studio 16 2019" -A x64 ..
```

If you're using a different version of Visual Studio, adjust the generator name accordingly (e.g., "Visual Studio 17 2022" for Visual Studio 2022).

**Step 4: Build the Plugin**  
Open the generated solution file in Visual Studio or build from the command line:

```cmd
cmake --build . --config Release
```

This will create the `veyon-chat-plugin.dll` file in the `Release` subdirectory.

**Step 5: Install the Built Plugin**  
Copy the compiled DLL file to the Veyon plugins directory as described in Method 1, Step 3.

## Configuration and Setup

After installing the plugin, you need to configure it for your classroom environment.

### Master Configuration

**Launch Veyon Master**: Start the Veyon Master application. You should now see a new "Chat" button in the toolbar or feature panel. If the chat feature is not visible, verify that the plugin was installed correctly and that Veyon was restarted after installation.

**Set Master Name**: The first time you use the chat feature, you will be prompted to set your master name. This name will be displayed to students in their chat windows. Choose a professional name like "Teacher" or your actual name.

**Test Chat Functionality**: Press F10 or click the Chat button to open the chat window. The interface should display a list of connected clients on the left and a chat area on the right.

### Client Configuration

**Automatic Detection**: Client computers should automatically detect the chat plugin when the Veyon Service starts. No manual configuration is typically required on client machines.

**F10 Hotkey**: Students can press F10 to open their chat window and communicate with the teacher. The chat window will also open automatically when they receive a message from the teacher.

**Notification Settings**: Sound notifications are enabled by default. Students can adjust notification settings through the chat window's system tray icon if they minimize the chat to the tray.

## Troubleshooting

If you encounter issues during installation or operation, try these common solutions.

### Plugin Not Loading

**Verify File Placement**: Ensure the `veyon-chat-plugin.dll` file is in the correct plugins directory and that you have the right version for your system architecture.

**Check Dependencies**: Verify that all required Visual C++ Redistributables are installed. Missing dependencies will prevent the plugin from loading properly.

**Review Veyon Logs**: Check the Veyon log files for error messages related to plugin loading. Logs are typically located in `%APPDATA%\Veyon\` or can be viewed through the Veyon Configurator.

### Chat Window Not Opening

**Keyboard Shortcut Conflicts**: If F10 doesn't work, check for conflicts with other applications that might be capturing this key combination. You can also access the chat through the Veyon Master toolbar.

**Firewall Issues**: Ensure that Windows Firewall or third-party security software is not blocking communication between the master and client computers.

**Network Configuration**: Verify that your Veyon network configuration allows for the additional chat message traffic between master and clients.

### Performance Issues

**Resource Usage**: The chat plugin is designed to be lightweight, but if you experience performance issues, check that your computers meet the minimum system requirements for Veyon and have sufficient available memory.

**Network Bandwidth**: In classrooms with many students, frequent chat messages might impact network performance. Consider using the global broadcast feature for announcements rather than individual messages when appropriate.

## Security Considerations

The chat plugin inherits Veyon's security model and adds additional considerations for classroom communication.

**Message Encryption**: All chat messages are transmitted using Veyon's existing encryption protocols, ensuring that communications remain secure and private within your classroom network.

**Access Control**: Only properly authenticated Veyon clients can participate in chat sessions. The plugin respects all existing Veyon access control rules and authentication requirements.

**Message Logging**: Chat messages are not permanently stored by default, but administrators should be aware that message content exists in memory during active sessions. Consider your institution's data retention and privacy policies when deploying the chat feature.

## Uninstallation

To remove the Veyon Chat Plugin from your system, follow these steps.

**Stop Veyon Services**: Ensure that all Veyon applications and services are stopped before removing the plugin files.

**Remove Plugin File**: Delete the `veyon-chat-plugin.dll` file from the Veyon plugins directory. You will need administrator privileges to perform this action.

**Restart Veyon**: Restart the Veyon Master and Service applications. The chat feature will no longer be available, and any existing chat windows will close automatically.

**Clean Configuration**: If desired, you can remove any chat-related configuration settings through the Veyon Configurator, though this is typically not necessary as the settings will simply be ignored without the plugin present.

## Support and Updates

For technical support, bug reports, or feature requests related to the Veyon Chat Plugin, please visit the project repository or contact the development team. Regular updates may be released to address issues or add new functionality, so check periodically for newer versions of the plugin.

When updating the plugin, follow the same installation procedure but ensure that you stop all Veyon services before replacing the DLL file with the newer version.
