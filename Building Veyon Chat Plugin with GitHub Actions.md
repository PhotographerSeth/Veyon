# Building Veyon Chat Plugin with GitHub Actions

**Author**: Manus AI  
**Date**: 2025-09-30  
**Method**: Automated cloud building using GitHub Actions

## Overview

This guide shows you how to use GitHub's free cloud building service to automatically compile the Veyon Chat Plugin DLL without installing any development tools on your computer. GitHub Actions will build the plugin for you and provide a downloadable DLL file.

## Prerequisites

You only need a free GitHub account. No development tools, Visual Studio, or Qt installation required on your computer.

**Create GitHub Account**: If you don't have one, sign up for free at [https://github.com](https://github.com). GitHub provides free Actions minutes for public repositories.

## Step-by-Step Instructions

### Step 1: Create a New Repository

**Navigate to GitHub**: Go to [https://github.com](https://github.com) and sign in to your account.

**Create Repository**: Click the green "New" button or go to [https://github.com/new](https://github.com/new).

**Repository Settings**: Configure your new repository with the following settings:
- **Repository name**: `veyon-chat-plugin`
- **Description**: `Free chat add-on for Veyon classroom management`
- **Visibility**: Public (required for free Actions minutes)
- **Initialize**: Check "Add a README file"
- **License**: Choose "GNU General Public License v2.0"

**Create Repository**: Click "Create repository" to finish setup.

### Step 2: Upload the Plugin Source Code

**Access Upload Interface**: In your new repository, click "uploading an existing file" or drag and drop files directly onto the repository page.

**Upload Files**: You need to upload all the source files I provided. The essential files are:

```
src/
├── ChatFeaturePlugin.h
├── ChatFeaturePlugin.cpp
├── ChatMasterWidget.h
├── ChatClientWidget.h
├── ChatMessage.h
├── ChatMessage.cpp
├── ChatSession.h
├── ChatSession.cpp
├── ChatServiceClient.h
└── ChatServiceClient.cpp

ui/
├── ChatMasterWidget.ui
└── ChatClientWidget.ui

resources/
├── chat.qrc
├── icons/
│   ├── chat.png
│   ├── send.png
│   ├── clear.png
│   ├── broadcast.png
│   ├── online.png
│   ├── away.png
│   └── typing.png
└── sounds/
    └── notification.wav

.github/
└── workflows/
    └── build-windows.yml

CMakeLists.txt
README.md
install-windows.bat
install-windows.ps1
```

**Upload Method Options**:

**Option A: Drag and Drop**
1. Extract the `veyon-chat-plugin.tar.gz` file I provided
2. Drag all folders and files to the GitHub repository page
3. GitHub will automatically upload and commit them

**Option B: Git Command Line** (if you have Git installed)
```bash
git clone https://github.com/yourusername/veyon-chat-plugin.git
cd veyon-chat-plugin
# Copy all source files here
git add .
git commit -m "Add Veyon Chat Plugin source code"
git push
```

**Option C: GitHub Web Interface**
1. Click "Add file" → "Upload files"
2. Drag the extracted source files to the upload area
3. Add commit message: "Add Veyon Chat Plugin source code"
4. Click "Commit changes"

### Step 3: Verify GitHub Actions Setup

**Check Workflow File**: Ensure the `.github/workflows/build-windows.yml` file is present in your repository. This file contains the automated build instructions.

**Actions Tab**: Click the "Actions" tab in your repository. You should see the workflow listed as "Build Veyon Chat Plugin for Windows".

**Initial Build**: GitHub Actions should automatically start building when you upload the source code. If not, you can manually trigger it by clicking "Run workflow" in the Actions tab.

### Step 4: Monitor the Build Process

**Build Status**: In the Actions tab, you'll see the build progress. The build typically takes 5-10 minutes to complete.

**Build Steps**: The automated process will:
1. Set up a Windows virtual machine
2. Install Qt framework and build tools
3. Configure the project with CMake
4. Compile the source code into a DLL
5. Package the DLL with installation scripts

**Build Logs**: Click on the running build to see detailed logs. This helps troubleshoot any issues.

### Step 5: Download the Compiled DLL

**Successful Build**: When the build completes successfully (green checkmark), scroll down to the "Artifacts" section.

**Download Options**: You'll see two downloadable packages:
- **veyon-chat-plugin-windows**: Contains just the DLL file and scripts
- **veyon-chat-plugin-release-package**: Complete package with DLL, installation scripts, and documentation

**Download Process**: Click on either artifact name to download a ZIP file containing the compiled plugin.

**Extract Files**: Unzip the downloaded file to get:
- `veyon-chat-plugin.dll` (the compiled plugin)
- `install-windows.bat` (installation script)
- `install-windows.ps1` (PowerShell installation script)
- `README.md` (usage instructions)

### Step 6: Install the Plugin

**Installation**: Now that you have the compiled DLL, follow the installation instructions:

1. Extract the downloaded ZIP file to a folder like `C:\VeyonChatPlugin\`
2. Open Command Prompt as Administrator
3. Navigate to the folder: `cd C:\VeyonChatPlugin\`
4. Run the installation script: `install-windows.bat`

The installation script will automatically copy the DLL to your Veyon plugins directory and restart the necessary services.

## Troubleshooting GitHub Actions

### Common Build Issues

**Build Fails with Qt Errors**: The GitHub Actions workflow automatically installs Qt, but sometimes specific versions may have issues. The workflow is configured to use Qt 5.15.2, which is stable and widely compatible.

**CMake Configuration Errors**: If the build fails during CMake configuration, it might be due to missing Veyon development headers. The workflow includes fallback options for this scenario.

**Upload Artifacts Failed**: If the build succeeds but artifact upload fails, you can still access the compiled DLL by downloading the build logs and extracting files from the log output.

### Manual Workflow Trigger

**Trigger Build Manually**: If the automatic build doesn't start, you can manually trigger it:
1. Go to the Actions tab in your repository
2. Click "Build Veyon Chat Plugin for Windows"
3. Click "Run workflow"
4. Select the branch (usually "main" or "master")
5. Click "Run workflow"

### Build Status Badges

**Add Status Badge**: You can add a build status badge to your README to show the current build status:

```markdown
![Build Status](https://github.com/yourusername/veyon-chat-plugin/workflows/Build%20Veyon%20Chat%20Plugin%20for%20Windows/badge.svg)
```

## Updating the Plugin

**Code Changes**: When you want to update the plugin, simply upload new source files to your GitHub repository. GitHub Actions will automatically build the updated version.

**Version Management**: Consider using Git tags or releases to manage different versions of your plugin:

1. Go to your repository's main page
2. Click "Releases" on the right sidebar
3. Click "Create a new release"
4. Tag the current version and upload the compiled DLL as a release asset

## Sharing Your Plugin

**Public Repository**: Since your repository is public, others can:
- View and download your source code
- Fork your repository to make their own modifications
- Submit issues and feature requests
- Contribute improvements via pull requests

**Distribution**: You can share the direct download link to your compiled DLL with other Veyon users who want to use the chat plugin.

## Cost Considerations

**Free Tier**: GitHub provides 2,000 Actions minutes per month for free on public repositories. Each build takes about 5-10 minutes, so you can build the plugin approximately 200-400 times per month at no cost.

**Private Repositories**: If you make your repository private, you'll have a smaller free allowance (500 minutes/month), but this is still sufficient for personal use.

This automated approach eliminates the need for local development tools while providing a professional, repeatable build process that others can use and contribute to.
