#!/bin/bash

# CR-717 VST3 Build and Install Script
# Builds the plugin and installs it to the system VST3 directory

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Detect OS
OS="$(uname -s)"
case "${OS}" in
    Darwin*)    PLATFORM="macOS";;
    Linux*)     PLATFORM="Linux";;
    MINGW*|MSYS*|CYGWIN*) PLATFORM="Windows";;
    *)          PLATFORM="Unknown";;
esac

echo -e "${BLUE}=== CR-717 VST3 Build and Install ===${NC}"
echo -e "${BLUE}Platform: ${PLATFORM}${NC}"
echo ""

# Set paths
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
VST_SOURCE_DIR="${SCRIPT_DIR}/native/vst3/CR717"
BUILD_DIR="${VST_SOURCE_DIR}/build"

# Determine VST3 install location based on platform
if [ "${PLATFORM}" = "macOS" ]; then
    VST3_INSTALL_DIR="${HOME}/Library/Audio/Plug-Ins/VST3"
    VST3_NAME="Cherni CR-717.vst3"
elif [ "${PLATFORM}" = "Linux" ]; then
    VST3_INSTALL_DIR="${HOME}/.vst3"
    VST3_NAME="Cherni CR-717.vst3"
elif [ "${PLATFORM}" = "Windows" ]; then
    VST3_INSTALL_DIR="/c/Program Files/Common Files/VST3"
    VST3_NAME="Cherni CR-717.vst3"
else
    echo -e "${RED}Unsupported platform: ${PLATFORM}${NC}"
    exit 1
fi

echo -e "${YELLOW}Source directory: ${VST_SOURCE_DIR}${NC}"
echo -e "${YELLOW}Build directory: ${BUILD_DIR}${NC}"
echo -e "${YELLOW}Install directory: ${VST3_INSTALL_DIR}${NC}"
echo ""

# Check if source directory exists
if [ ! -d "${VST_SOURCE_DIR}" ]; then
    echo -e "${RED}Error: VST source directory not found: ${VST_SOURCE_DIR}${NC}"
    exit 1
fi

# Navigate to VST source directory
cd "${VST_SOURCE_DIR}"

# Clean previous build if requested
if [ "$1" = "clean" ] || [ "$1" = "--clean" ]; then
    echo -e "${YELLOW}Cleaning previous build...${NC}"
    if [ -d "${BUILD_DIR}" ]; then
        rm -rf "${BUILD_DIR}"
        echo -e "${GREEN}✓ Build directory cleaned${NC}"
    fi
    echo ""
fi

# Create build directory
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Configure with CMake
echo -e "${BLUE}Configuring CMake...${NC}"
cmake -DCMAKE_BUILD_TYPE=Release ..

if [ $? -ne 0 ]; then
    echo -e "${RED}✗ CMake configuration failed${NC}"
    exit 1
fi
echo -e "${GREEN}✓ CMake configuration successful${NC}"
echo ""

# Build
echo -e "${BLUE}Building plugin...${NC}"
NUM_CORES=$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)
cmake --build . --config Release -j${NUM_CORES}

if [ $? -ne 0 ]; then
    echo -e "${RED}✗ Build failed${NC}"
    exit 1
fi
echo -e "${GREEN}✓ Build successful${NC}"
echo ""

# Find the built VST3
if [ "${PLATFORM}" = "macOS" ]; then
    BUILT_VST="${BUILD_DIR}/CR717_artefacts/Release/VST3/${VST3_NAME}"
elif [ "${PLATFORM}" = "Linux" ]; then
    BUILT_VST="${BUILD_DIR}/CR717_artefacts/Release/VST3/${VST3_NAME}"
elif [ "${PLATFORM}" = "Windows" ]; then
    BUILT_VST="${BUILD_DIR}/CR717_artefacts/Release/VST3/${VST3_NAME}"
fi

# Check if built VST exists
if [ ! -e "${BUILT_VST}" ]; then
    echo -e "${RED}✗ Built VST3 not found at: ${BUILT_VST}${NC}"
    echo -e "${YELLOW}Searching for VST3 in build directory...${NC}"
    find "${BUILD_DIR}" -name "*.vst3" -type d -o -name "*.vst3" -type f
    exit 1
fi

echo -e "${GREEN}✓ Found built VST3: ${BUILT_VST}${NC}"
echo ""

# Create install directory if it doesn't exist
echo -e "${BLUE}Preparing installation...${NC}"
mkdir -p "${VST3_INSTALL_DIR}"

# Remove existing installation
INSTALLED_VST="${VST3_INSTALL_DIR}/${VST3_NAME}"
if [ -e "${INSTALLED_VST}" ]; then
    echo -e "${YELLOW}Removing existing installation...${NC}"
    rm -rf "${INSTALLED_VST}"
    echo -e "${GREEN}✓ Existing installation removed${NC}"
fi

# Install (copy) the VST3
echo -e "${BLUE}Installing plugin...${NC}"
cp -R "${BUILT_VST}" "${VST3_INSTALL_DIR}/"

if [ $? -ne 0 ]; then
    echo -e "${RED}✗ Installation failed${NC}"
    exit 1
fi

echo -e "${GREEN}✓ Plugin installed successfully${NC}"
echo ""

# Verify installation
if [ -e "${INSTALLED_VST}" ]; then
    echo -e "${GREEN}=== Installation Complete ===${NC}"
    echo -e "${GREEN}Plugin location: ${INSTALLED_VST}${NC}"
    echo ""
    echo -e "${BLUE}Next steps:${NC}"
    echo -e "  1. Restart your DAW (Ableton Live, Reaper, etc.)"
    echo -e "  2. Rescan plugins if necessary"
    echo -e "  3. Look for 'Cherni CR-717' in your instrument list"
    echo ""
    
    # macOS specific: check code signing
    if [ "${PLATFORM}" = "macOS" ]; then
        echo -e "${YELLOW}Note: For distribution, you should code sign the plugin:${NC}"
        echo -e "  codesign --deep --force --sign \"Developer ID\" \"${INSTALLED_VST}\""
        echo ""
    fi
else
    echo -e "${RED}✗ Installation verification failed${NC}"
    exit 1
fi

# Optional: Run pluginval if available
if command -v pluginval &> /dev/null; then
    echo -e "${BLUE}pluginval found. Run validation? (y/n)${NC}"
    read -r response
    if [ "$response" = "y" ] || [ "$response" = "Y" ]; then
        echo -e "${BLUE}Running pluginval...${NC}"
        pluginval --strictness-level 5 --validate "${INSTALLED_VST}"
    fi
else
    echo -e "${YELLOW}Tip: Install pluginval for automated testing:${NC}"
    echo -e "  https://github.com/Tracktion/pluginval${NC}"
fi

echo ""
echo -e "${GREEN}Done!${NC}"
