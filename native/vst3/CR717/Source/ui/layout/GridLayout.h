#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <array>

/**
 * Professional 12-column grid layout system
 * Based on modern responsive design principles
 * Supports multiple breakpoints and fluid scaling
 */
class GridLayout
{
public:
    // Grid constants
    static constexpr int COLUMNS = 12;
    static constexpr int BASE_GUTTER = 8;
    static constexpr int BASE_MARGIN = 8;
    
    // Breakpoint definitions
    enum class Breakpoint
    {
        Small,   // 1100x720  - Minimum size
        Medium,  // 1440x900  - Preferred size
        Large    // 1680x980+ - Maximum tested
    };
    
    struct BreakpointConfig
    {
        int minWidth;
        int headerHeight;
        int kitHeight;
        int gutter;
        int margin;
        float scale;
        
        static BreakpointConfig forBreakpoint(Breakpoint bp)
        {
            switch (bp)
            {
                case Breakpoint::Small:
                    return {1100, 56, 104, 8, 8, 1.0f};
                case Breakpoint::Medium:
                    return {1440, 64, 120, 8, 8, 1.0f};
                case Breakpoint::Large:
                    return {1680, 72, 128, 8, 8, 1.0f};
            }
            return {1440, 64, 120, 8, 8, 1.0f};
        }
    };
    
    GridLayout(int totalWidth, int totalHeight)
        : width(totalWidth)
        , height(totalHeight)
    {
        updateBreakpoint();
        calculateGrid();
    }
    
    // Get rectangle for column span
    juce::Rectangle<int> getRect(int col, int span, int y, int h) const
    {
        jassert(col >= 0 && col < COLUMNS);
        jassert(span > 0 && col + span <= COLUMNS);
        
        int x = config.margin + col * (columnWidth + config.gutter);
        int w = span * columnWidth + (span - 1) * config.gutter;
        return {x, y, w, h};
    }
    
    // Get full-width rectangle (all 12 columns)
    juce::Rectangle<int> getFullWidth(int y, int h) const
    {
        return getRect(0, COLUMNS, y, h);
    }
    
    // Get left section (9 columns for voice strips)
    juce::Rectangle<int> getLeftSection(int y, int h) const
    {
        return getRect(0, 9, y, h);
    }
    
    // Get right section (3 columns for master/FX)
    juce::Rectangle<int> getRightSection(int y, int h) const
    {
        return getRect(9, 3, y, h);
    }
    
    // Layout zones with proper spacing
    struct LayoutZones
    {
        juce::Rectangle<int> header;
        juce::Rectangle<int> kit;
        juce::Rectangle<int> voiceArea;
        juce::Rectangle<int> masterPanel;
    };
    
    LayoutZones getZones() const
    {
        LayoutZones zones;
        
        int y = config.margin;
        
        // Header (full width)
        zones.header = getFullWidth(y, config.headerHeight);
        y += config.headerHeight + 16; // 16px spacing between major zones
        
        // Kit & Patterns (full width)
        zones.kit = getFullWidth(y, config.kitHeight);
        y += config.kitHeight + 16;
        
        // Voice area (left 9 columns)
        int voiceHeight = height - y - config.margin;
        zones.voiceArea = getLeftSection(y, voiceHeight);
        
        // Master panel (right 3 columns, from header to bottom)
        int masterY = config.margin + config.headerHeight + 16;
        int masterHeight = height - masterY - config.margin;
        zones.masterPanel = getRightSection(masterY, masterHeight);
        
        return zones;
    }
    
    // Get individual column position
    int getColumnX(int col) const
    {
        return config.margin + col * (columnWidth + config.gutter);
    }
    
    // Get column width
    int getColumnWidth() const { return columnWidth; }
    
    // Get gutter size
    int getGutter() const { return config.gutter; }
    
    // Get margin size
    int getMargin() const { return config.margin; }
    
    // Get current breakpoint
    Breakpoint getBreakpoint() const { return currentBreakpoint; }
    
    // Get breakpoint config
    const BreakpointConfig& getConfig() const { return config; }
    
    // Update for new size
    void resize(int newWidth, int newHeight)
    {
        width = newWidth;
        height = newHeight;
        updateBreakpoint();
        calculateGrid();
    }
    
    // Get content bounds (inside margins)
    juce::Rectangle<int> getContentBounds() const
    {
        return {config.margin, config.margin, 
                width - 2 * config.margin, 
                height - 2 * config.margin};
    }
    
    // Helper: distribute space evenly across columns
    std::array<int, COLUMNS> getColumnPositions() const
    {
        std::array<int, COLUMNS> positions;
        for (int i = 0; i < COLUMNS; ++i)
            positions[i] = getColumnX(i);
        return positions;
    }
    
    // Helper: get spacing between major zones
    static constexpr int getMajorZoneSpacing() { return 16; }
    static constexpr int getMinorZoneSpacing() { return 8; }
    
private:
    void updateBreakpoint()
    {
        if (width < 1280)
            currentBreakpoint = Breakpoint::Small;
        else if (width < 1600)
            currentBreakpoint = Breakpoint::Medium;
        else
            currentBreakpoint = Breakpoint::Large;
        
        config = BreakpointConfig::forBreakpoint(currentBreakpoint);
    }
    
    void calculateGrid()
    {
        // Calculate column width with proper rounding
        int availableWidth = width - 2 * config.margin - (COLUMNS - 1) * config.gutter;
        columnWidth = availableWidth / COLUMNS;
        
        // Store remainder for last column adjustment if needed
        remainder = availableWidth - (columnWidth * COLUMNS);
    }
    
    int width;
    int height;
    int columnWidth = 0;
    int remainder = 0;
    Breakpoint currentBreakpoint = Breakpoint::Medium;
    BreakpointConfig config;
};

/**
 * Layout helper for common patterns
 */
namespace LayoutHelpers
{
    // Create evenly spaced items in a container
    inline std::vector<juce::Rectangle<int>> distributeHorizontally(
        const juce::Rectangle<int>& container,
        int itemCount,
        int spacing)
    {
        std::vector<juce::Rectangle<int>> rects;
        if (itemCount <= 0) return rects;
        
        int totalSpacing = (itemCount - 1) * spacing;
        int itemWidth = (container.getWidth() - totalSpacing) / itemCount;
        
        for (int i = 0; i < itemCount; ++i)
        {
            int x = container.getX() + i * (itemWidth + spacing);
            rects.push_back({x, container.getY(), itemWidth, container.getHeight()});
        }
        
        return rects;
    }
    
    // Create evenly spaced items vertically
    inline std::vector<juce::Rectangle<int>> distributeVertically(
        const juce::Rectangle<int>& container,
        int itemCount,
        int spacing)
    {
        std::vector<juce::Rectangle<int>> rects;
        if (itemCount <= 0) return rects;
        
        int totalSpacing = (itemCount - 1) * spacing;
        int itemHeight = (container.getHeight() - totalSpacing) / itemCount;
        
        for (int i = 0; i < itemCount; ++i)
        {
            int y = container.getY() + i * (itemHeight + spacing);
            rects.push_back({container.getX(), y, container.getWidth(), itemHeight});
        }
        
        return rects;
    }
    
    // Create grid of items
    inline std::vector<juce::Rectangle<int>> createGrid(
        const juce::Rectangle<int>& container,
        int rows,
        int cols,
        int hSpacing,
        int vSpacing)
    {
        std::vector<juce::Rectangle<int>> rects;
        
        int totalHSpacing = (cols - 1) * hSpacing;
        int totalVSpacing = (rows - 1) * vSpacing;
        int itemWidth = (container.getWidth() - totalHSpacing) / cols;
        int itemHeight = (container.getHeight() - totalVSpacing) / rows;
        
        for (int row = 0; row < rows; ++row)
        {
            for (int col = 0; col < cols; ++col)
            {
                int x = container.getX() + col * (itemWidth + hSpacing);
                int y = container.getY() + row * (itemHeight + vSpacing);
                rects.push_back({x, y, itemWidth, itemHeight});
            }
        }
        
        return rects;
    }
}
