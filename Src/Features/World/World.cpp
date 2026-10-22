#include "World.h"
#include "./Puzzle/PuzzleRenderer.h"

World::World(const StageConfig& config) : map(config), stage(config)
{
    spawner.configure(config.enemy);
}

Map& World::getMap() { return map; }
const Map& World::getMap() const { return map; }
Stage& World::getStage() { return stage; }
const Stage& World::getStage() const { return stage; }
EnemySpawner& World::getSpawner() { return spawner; }

Vector2 World::getFocus() const { return camera.getPosition(); }
void World::setFocus(const Vector2& focus) { camera.setPosition(focus); }

Engine::Graphics::CameraController& World::getCamera() { return camera; }
const Engine::Graphics::CameraController& World::getCamera() const { return camera; }

void World::update(float deltaTime, const Vector2& playerPosition, std::vector<Target>& targets)
{
    camera.refreshViewport();
    followDeadzone(camera, playerPosition);

    exploreCenter = playerPosition;
    map.loadChunks(playerPosition, exploreRadius);

    map.update(deltaTime);

    // The stage decides how many enemies appear, the spawner places them, kills flow back.
    const int spawnCount = stage.update(deltaTime, spawner.getAlive(), map.allGatesOpen());
    spawner.update(deltaTime, targets, playerPosition, exploreRadius, spawnCount, map);

    stage.registerKills(spawner.consumeKills());
    map.notifyKills(stage.getKills());
}

void World::drawChunks(Engine::Graphics::Renderer& renderer) const
{
    const auto visible = [&](const Chunk& chunk) {
        Rectangle r = chunk.getBounds();
        r.x -= 16.0f; r.y -= 16.0f; r.width += 32.0f; r.height += 32.0f;
        return renderer.isVisible(r);
        };

    for (const Chunk& chunk : map.getExploredChunks()) {
        if (visible(chunk)) chunk.renderGround(renderer, exploreCenter, exploreRadius);
    }
    for (const Chunk& chunk : map.getExploredChunks()) {
        if (visible(chunk)) chunk.renderObjects(renderer, exploreCenter, exploreRadius);
    }
}

void World::drawEntities(Engine::Graphics::Renderer& renderer) const
{
    PuzzleRenderer::draw(renderer, map.getItems(), map.getGates(),
        static_cast<float>(Cell::Size), exploreCenter, exploreRadius);

    if (!map.isPrisonerFreed()) {
        const Vector2 p = map.getPrisonerPosition();
        const float dx = p.x - exploreCenter.x;
        const float dy = p.y - exploreCenter.y;

        if (dx * dx + dy * dy <= exploreRadius * exploreRadius) {
            renderer.drawCircle(p, 6.0f, Color{ 40, 170, 170, 255 });
            renderer.drawCircleOutline(p, 6.0f, WHITE);

            const Rectangle cage = map.getCageBounds();
            const Color iron{ 50, 50, 58, 255 };
            for (int i = 0; i <= 4; ++i) {
                const float x = cage.x + cage.width * i / 4.0f;
                renderer.drawLine(Vector2{ x, cage.y }, Vector2{ x, cage.y + cage.height }, 1.5f, iron);
            }
            renderer.drawRectangleOutline(cage, 2.0f, iron);
        }
    }

    spawner.render(renderer, exploreCenter, exploreRadius);
}

void World::collectTargets(std::vector<Enemy*>& out)
{
    spawner.collectTargets(out, exploreCenter, exploreRadius);
}

void World::followDeadzone(Engine::Graphics::CameraController& camera, const Vector2& target)
{
    const float zoom = camera.getCamera().zoom;
    const float halfWidth = GetScreenWidth() * 0.5f * deadzone / zoom;
    const float halfHeight = GetScreenHeight() * 0.5f * deadzone / zoom;

    Vector2 center = camera.getPosition();

    if (target.x < center.x - halfWidth)       center.x = target.x + halfWidth;
    else if (target.x > center.x + halfWidth)  center.x = target.x - halfWidth;

    if (target.y < center.y - halfHeight)      center.y = target.y + halfHeight;
    else if (target.y > center.y + halfHeight) center.y = target.y - halfHeight;

    camera.setPosition(center);
}