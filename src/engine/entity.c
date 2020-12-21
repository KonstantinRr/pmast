// MIT License
// 
// Copyright (c) 2020 Konstantin Rolf
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <engine/engine.h>

void TileAtlasCreate(TileAtlas *atlas, const Texture2D *texture, int elemSize)
{
    atlas->texture = texture;
    atlas->elemWidth = texture->width / elemSize;
    atlas->elemHeight = texture->height / elemSize;
    atlas->elemSize = elemSize;
}

int TileAtlasGetIndex(const TileAtlas *atlas, int x, int y)
{
    return y * atlas->elemWidth + x;
}

int TileAtlasGetPosXIndex(const TileAtlas *atlas, int tile)
{
    return tile % atlas->elemWidth;
}

int TileAtlasGetPosYIndex(const TileAtlas *atlas, int tile)
{
    return tile / atlas->elemWidth;
}

int TileAtlasGetPosX(const TileAtlas *atlas, int tile)
{
    return (tile % atlas->elemWidth) * atlas->elemSize;
}

int TileAtlasGetPosY(const TileAtlas *atlas, int tile)
{
    return (tile / atlas->elemWidth) * atlas->elemSize;
}

float TileAtlasGetPosXScaled(const TileAtlas *atlas, int tile)
{
    return (float)TileAtlasGetPosX(atlas, tile) / atlas->texture->width;
}

float TileAtlasGetPosYScaled(const TileAtlas *atlas, int tile)
{
    return (float)TileAtlasGetPosY(atlas, tile) / atlas->texture->height;
}

// ---- Entity ---- //

void EntityCreate(Entity *entity, float positionX, float positionY, float rotation, float scale)
{
    entity->positionX = positionX;
    entity->positionY = positionY;
    entity->rotation = rotation;
    entity->scale = scale;
    entity->tileIndex = 0;
    entity->atlas = 0;
}

void EntityCreateTiling(Entity *entity, int tileIndex, const TileAtlas *atlas,
    float positionX, float positionY, float rotation, float scale)
{
    entity->positionX = positionX;
    entity->positionY = positionY;
    entity->rotation = rotation;
    entity->scale = scale;
    entity->tileIndex = tileIndex;
    entity->atlas = atlas;
}

// ---- EntityQueue ---- //

void QueueCreate(EntityQueue *queue)
{
    queue->current = 0;
}
void QueueClear(EntityQueue *queue)
{
    queue->current = 0;
}

Entity* QueuePointer(EntityQueue *queue)
{
    return &(queue->queue[queue->current]);
}

Entity* QueuePointerNext(EntityQueue *queue)
{
    return &(queue->queue[queue->current++]);
}

void QueuePush(EntityQueue *queue, Entity *entity)
{
    queue->queue[queue->current] = *entity;
    queue->current++;
}

void QueuePop(EntityQueue *queue)
{
    queue->current--;
    if (queue->current < 0)
        queue->current = 0;
}