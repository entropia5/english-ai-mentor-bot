#pragma once
#include <filesystem>
#include <cstdint>
#include <mutex>
std::mutex& render_cache_mutex();

// Только созданные рендерером файлы; свежие карточки защищены от удаления.
void prune_render_cache(const std::filesystem::path& directory,
                        std::uintmax_t budget = 256ULL * 1024 * 1024);
void maintain_render_cache();
