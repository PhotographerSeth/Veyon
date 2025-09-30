# Veyon Research Notes

## Project Overview
- **Name**: Veyon (Virtual Eye On Networks)
- **License**: GNU GPL v2
- **Language**: Primarily C++ (82.6%), with some C (10.8%) and CMake (4.2%)
- **Platform**: Cross-platform (Windows, Linux)
- **Purpose**: Computer monitoring and classroom management software

## Key Features
- Overview: monitor all computers in one or multiple locations
- Remote access: view or control computers
- Demo: broadcast teacher's screen in realtime
- Screen lock: draw attention
- **Communication: send text messages to students** (existing feature!)
- Start/end lessons: log in/out users
- Screenshots: record progress
- Programs & websites: launch remotely
- Teaching material: distribute documents
- Administration: power on/off and reboot

## Project Structure (from GitHub)
Key directories:
- `/core` - Core functionality
- `/master` - Master application (teacher/admin interface)
- `/server` - Server component (runs on client machines)
- `/plugins` - Plugin system
- `/configurator` - Configuration tool
- `/cli` - Command line interface
- `/service` - Service components

## Architecture Notes
- Uses Qt5/Qt6 framework
- Has a plugin system (plugins directory exists)
- Master-server architecture (master controls multiple servers/clients)
- Cross-platform with X11 support on Linux
- Uses VNC technology (x11vnc submodule)

## Existing Communication Feature
Veyon already has "Communication: send text messages to students" - need to investigate this further to understand current implementation and see if we can extend it.

## Next Steps
1. Examine the plugins directory structure
2. Look at existing communication feature implementation
3. Check developer documentation
4. Analyze plugin development guidelines


## Existing Text Message Plugin Analysis

Found that Veyon already has a **textmessage** plugin with the following components:

### Plugin Structure
- `TextMessageFeaturePlugin.cpp/.h` - Main plugin implementation
- `TextMessageDialog.cpp/.h/.ui/.qml` - Dialog interface for sending messages
- `CMakeLists.txt` - Build configuration
- `textmessage.qrc` - Resource file
- `dialog-information.png` - Icon resource

### Key Implementation Details
- Uses Qt framework with both traditional UI (.ui) and QML (.qml) interfaces
- Implements `FeaturePlugin` base class
- Has `TextMessageFeature` with UUID "e75ae9c8-ac17-4d00-bf6d-019348346280"
- Supports sending text messages from master to clients
- Uses `sendFeatureMessage()` to communicate with clients
- Arguments include text content and icon for the message

### Current Limitations (for our chat requirements)
- Appears to be one-way communication (master to client only)
- No persistent chat history
- No client-to-master communication
- No F10 hotkey activation
- No global broadcast specifically mentioned
- No chat clearing functionality

This existing plugin provides a good foundation but needs significant extension for bidirectional chat functionality.

## Commercial Chat Add-on Analysis

Veyon already has a **commercial Chat add-on** that provides enhanced chat functionality:

### Commercial Chat Features
- Adds another channel for classroom communication
- Allows assignments to be discussed in class
- Individual student participation without distractions
- Chat window remains permanently open in background
- Participation possible at any time

### Implications for Our Project
Since Veyon already offers a commercial chat solution, our open-source chat add-on should:
1. Provide similar core functionality for free
2. Focus on the specific requirements mentioned (F10 hotkey, Master name setting, etc.)
3. Be clearly differentiated as a community/open-source alternative
4. Avoid directly competing with commercial features but provide basic chat needs

### Key Differences for Our Implementation
- F10 hotkey activation (not mentioned in commercial version)
- Master name customization
- Client computer name display
- Global message broadcasting
- Chat clearing functionality
- Bidirectional communication (client to master)

This validates that there is demand for chat functionality in Veyon, and our implementation can serve users who need basic chat features without purchasing the commercial add-on.

## Veyon Feature Class Architecture Analysis

### Feature Class Structure (from Feature.h)
The core Feature class in Veyon defines the following key components:

**Feature Flags Enum:**
- None = 0x0000
- Mode = 0x0001  
- Action = 0x0002
- Session = 0x0004
- Meta = 0x0008
- Option = 0x0010
- Checked = 0x0020
- Master = 0x0100
- Service = 0x0200
- Worker = 0x0400
- Builtin = 0x1000
- AllComponents = Master | Service | Worker

**Feature Constructor Parameters:**
- Uid (unique identifier)
- Flags
- QString name
- QString displayName
- QString displayNameActive
- QString description
- QString iconUrl
- QKeySequence shortcut

### Key Insights for Chat Plugin Development
1. **Feature System**: Veyon uses a feature-based plugin architecture where each feature has a unique UUID
2. **Component Flags**: Features can target Master, Service, and/or Worker components
3. **Built-in Support**: Features can be marked as Builtin or external plugins
4. **Keyboard Shortcuts**: Native support for QKeySequence shortcuts (perfect for F10 requirement)
5. **Display Properties**: Features have display names, descriptions, and icons
6. **Action/Mode System**: Features can be actions or modes with different behaviors

### Plugin Interface Requirements
Based on the textmessage plugin analysis, our chat plugin will need:
- FeaturePlugin base class implementation
- Feature definitions with appropriate flags
- Master component for teacher interface
- Service/Worker components for client functionality
- Message handling via FeatureMessage system
