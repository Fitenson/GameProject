#include <cmath>
#include "Chunk.h"

namespace {
    bool insideCircle(const Vector2& p, const Vector2& center, float radius)
    {
        const float dx = p.x - center.x, dy = p.y - center.y;
        return dx * dx + dy * dy <= radius * radius;
    }
}

Chunk::Chunk(const Vector2& position, const std::vector<TileType>& tiles) : position(position)
{
    cells.reserve(Width * Height);

    for (uint32_t y = 0; y < Height; ++y)
    {
        for (uint32_t x = 0; x < Width; ++x)
        {
            const size_t index = static_cast<size_t>(y) * Width + x;
            const TileType type = index < tiles.size() ? tiles[index] : TileType::Grass;
            cells.emplace_back(Vector2{ static_cast<float>(x), static_cast<float>(y) }, type);
        }
    }
}

std::vector<Cell>& Chunk::getCells() { return cells; }

Vector2& Chunk::getPosition() { return position; }

const Vector2& Chunk::getPosition() const { return position; }

Rectangle Chunk::getBounds() const {
    const float width = static_cast<float>(Width * Cell::Size);
    const float height = static_cast<float>(Height * Cell::Size);
    return Rectangle{ position.x * width, position.y * height, width, height };
}

// Pass 1: flat things (ground, walls, gates, grass specks).
// Levers only get their dirt tile here; the lever itself is drawn in pass 2.
void Chunk::renderGround(Engine::Graphics::Renderer& renderer, const Vector2& center, float radius) const
{
    const Rectangle bounds = getBounds();
    const float size = static_cast<float>(Cell::Size);

    for (const Cell& cell : cells)
    {
        const Vector2& local = cell.getPosition();
        const Rectangle rect{ bounds.x + local.x * size, bounds.y + local.y * size, size, size };
        const Vector2 cellCenter{ rect.x + size * 0.5f, rect.y + size * 0.5f };
        if (!insideCircle(cellCenter, center, radius)) continue;

        const int worldX = static_cast<int>(position.x * Width) + static_cast<int>(local.x);
        const int worldY = static_cast<int>(position.y * Height) + static_cast<int>(local.y);
        const uint32_t hash = hashCell(worldX, worldY);
        const TileType type = cell.getType();

        Color ground;
        if (type == TileType::Wall) {
            const unsigned char v = static_cast<unsigned char>(hash % 18);
            ground = Color{ static_cast<unsigned char>(88 + v), static_cast<unsigned char>(90 + v),
                            static_cast<unsigned char>(96 + v), 255 };
        }
        else if (type == TileType::Path || type == TileType::Gate || type == TileType::Cage ||
            type == TileType::SwitchOff || type == TileType::SwitchOn) {
            const unsigned char v = static_cast<unsigned char>((hash >> 8) % 24);
            ground = Color{ static_cast<unsigned char>(125 + v), static_cast<unsigned char>(87 + v),
                            static_cast<unsigned char>(49 + v / 2), 255 };
        }
        else {
            const unsigned char v = static_cast<unsigned char>(hash % 35);
            ground = Color{ static_cast<unsigned char>(28 + v / 4), static_cast<unsigned char>(105 + v),
                            static_cast<unsigned char>(35 + v / 3), 255 };
        }
        renderer.drawRectangle(rect, ground);

        if (type == TileType::Wall) {
            renderer.drawRectangleOutline(rect, 1.0f, Color{ 60, 62, 68, 255 });
        }
        else if (type == TileType::Gate) {
            renderer.drawRectangle(rect, Color{ 110, 70, 35, 255 });
            renderer.drawRectangle(Rectangle{ rect.x, rect.y + size * 0.4f, size, 2.0f }, Color{ 55, 55, 62, 255 });
            renderer.drawRectangleOutline(rect, 1.0f, Color{ 45, 30, 15, 255 });
        }
        else if (type == TileType::Grass && (hash % 100) < 12) {
            renderer.drawCircle(cellCenter, 0.8f, Color{ 130, 175, 65, 255 });
        }
    }
}

// Pass 2: trees, rocks and levers, drawn over the neighbouring cells so tall objects overlap.
void Chunk::renderObjects(Engine::Graphics::Renderer& renderer, const Vector2& center, float radius) const
{
    const Rectangle bounds = getBounds();
    const float size = static_cast<float>(Cell::Size);

    for (const Cell& cell : cells)   // rows run top to bottom, so lower objects overlap higher ones
    {
        const TileType type = cell.getType();
        if (type != TileType::Tree && type != TileType::Rock &&
            type != TileType::SwitchOff && type != TileType::SwitchOn) continue;

        const Vector2& local = cell.getPosition();
        const Vector2 cellCenter{ bounds.x + (local.x + 0.5f) * size, bounds.y + (local.y + 0.5f) * size };
        if (!insideCircle(cellCenter, center, radius)) continue;

        const int worldX = static_cast<int>(position.x * Width) + static_cast<int>(local.x);
        const int worldY = static_cast<int>(position.y * Height) + static_cast<int>(local.y);
        const uint32_t hash = hashCell(worldX, worldY);

        const float scale = 0.9f + static_cast<float>((hash >> 20) % 30) / 100.0f;   // 0.9 .. 1.19
        const float jitterX = static_cast<float>(static_cast<int>((hash >> 8) % 5) - 2) * 0.5f;

        if (type == TileType::Tree) {
            const float r = size * 0.85f * scale;                        // canopy radius
            const float baseY = cellCenter.y + size * 0.4f;              // where the trunk meets the ground
            const Vector2 canopy{ cellCenter.x + jitterX, baseY - r * 0.85f };

            renderer.drawCircle(Vector2{ canopy.x + 2.0f, baseY - 1.0f }, r * 0.85f, Color{ 15, 30, 15, 90 });       // shadow
            renderer.drawRectangle(Rectangle{ canopy.x - 1.5f, baseY - 6.0f, 3.0f, 6.0f }, Color{ 95, 62, 33, 255 }); // trunk
            renderer.drawCircle(canopy, r, Color{ 15, 70, 28, 255 });                                                // dark base
            renderer.drawCircle(Vector2{ canopy.x - r * 0.25f, canopy.y - r * 0.15f }, r * 0.72f, Color{ 28, 105, 40, 255 });
            renderer.drawCircle(Vector2{ canopy.x - r * 0.4f, canopy.y - r * 0.4f }, r * 0.38f, Color{ 55, 140, 60, 255 }); // highlight
        }
        else if (type == TileType::Rock) {
            const float r = size * 0.55f * scale;
            const Vector2 rock{ cellCenter.x + jitterX, cellCenter.y + 1.0f };

            renderer.drawCircle(Vector2{ rock.x + 1.5f, rock.y + r * 0.7f }, r, Color{ 15, 30, 15, 100 });           // shadow
            renderer.drawCircle(rock, r, Color{ 95, 100, 98, 255 });
            renderer.drawCircle(Vector2{ rock.x + r * 0.55f, rock.y + r * 0.25f }, r * 0.6f, Color{ 80, 85, 84, 255 }); // second lump
            renderer.drawCircle(Vector2{ rock.x - r * 0.3f, rock.y - r * 0.35f }, r * 0.4f, Color{ 150, 155, 148, 255 }); // highlight
        }
        else {
            // Lever: stone base plate, slot, pivot, metal rod and knob.
            const bool on = (type == TileType::SwitchOn);
            const float baseY = cellCenter.y + size * 0.5f;

            const Rectangle plate{ cellCenter.x - 8.0f, baseY - 6.0f, 16.0f, 7.0f };
            renderer.drawRectangle(Rectangle{ plate.x + 1.5f, plate.y + 2.0f, plate.width, plate.height }, Color{ 15, 30, 15, 90 });   // shadow
            renderer.drawRectangle(plate, Color{ 88, 92, 100, 255 });
            renderer.drawRectangle(Rectangle{ plate.x, plate.y, plate.width, 2.0f }, Color{ 135, 140, 150, 255 });                   // top edge
            renderer.drawRectangleOutline(plate, 1.0f, Color{ 40, 42, 48, 255 });
            renderer.drawRectangle(Rectangle{ cellCenter.x - 5.0f, baseY - 4.0f, 10.0f, 2.0f }, Color{ 28, 28, 32, 255 });           // slot

            // The rod swings from the pivot: left = off, right = on.
            const Vector2 pivot{ cellCenter.x, baseY - 3.0f };
            const float angle = on ? 0.65f : -0.65f;                                   // radians from straight up
            const Vector2 tip{ pivot.x + std::sin(angle) * 13.0f, pivot.y - std::cos(angle) * 13.0f };

            const Color knob = on ? Color{ 80, 230, 110, 255 } : Color{ 225, 70, 60, 255 };
            if (on) renderer.drawCircle(tip, 7.0f, Color{ 80, 230, 110, 60 });         // soft glow

            renderer.drawLine(pivot, tip, 3.0f, Color{ 150, 154, 164, 255 });          // rod
            renderer.drawLine(pivot, tip, 1.0f, Color{ 215, 218, 226, 255 });          // rod highlight
            renderer.drawCircle(tip, 3.5f, knob);                                      // knob
            renderer.drawCircle(Vector2{ tip.x - 1.0f, tip.y - 1.0f }, 1.2f, Color{ 255, 255, 255, 180 });
            renderer.drawCircle(pivot, 2.2f, Color{ 60, 62, 70, 255 });                // pivot bolt
        }
    }
}