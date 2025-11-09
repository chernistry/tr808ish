#!/bin/bash
# Run pluginval on CR-717 VST3 plugin

set -e

PLUGIN_PATH="$HOME/Library/Audio/Plug-Ins/VST3/Cherni CR-717.vst3"
PLUGINVAL="/usr/local/bin/pluginval"

if [ ! -f "$PLUGINVAL" ]; then
    echo "❌ pluginval not found at $PLUGINVAL"
    echo "Install from: https://github.com/Tracktion/pluginval/releases"
    exit 1
fi

if [ ! -d "$PLUGIN_PATH" ]; then
    echo "❌ Plugin not found at $PLUGIN_PATH"
    echo "Run ./build-and-install-vst.sh first"
    exit 1
fi

echo "🧪 Running pluginval on CR-717..."
echo "Plugin: $PLUGIN_PATH"
echo ""

$PLUGINVAL --strictness-level 5 --validate-in-process --output-dir ./scripts/qa/results "$PLUGIN_PATH"

echo ""
echo "✅ pluginval complete. Check ./scripts/qa/results for detailed logs."
