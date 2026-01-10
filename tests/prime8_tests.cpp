#define BOOST_TEST_MODULE ProjectPrime8 Test Suite
#include <boost/test/included/unit_test.hpp>

#include <filesystem>
#include <fstream>
#include <system_error>

namespace fs = std::filesystem;

static fs::path root()
{
    return fs::path(PROJECT_ROOT);
}

static bool exists_file_not_empty(const fs::path& p)
{
    std::error_code ec;
    if (!fs::exists(p, ec) || !fs::is_regular_file(p, ec)) return false;
    return fs::file_size(p, ec) > 0;
}

static bool has_png_signature(const fs::path& p)
{
    std::ifstream f(p, std::ios::binary);
    if (!f) return false;

    unsigned char sig[8];
    f.read(reinterpret_cast<char*>(sig), 8);
    if (f.gcount() != 8) return false;

    const unsigned char pngSig[8] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    for (int i = 0; i < 8; ++i)
        if (sig[i] != pngSig[i]) return false;

    return true;
}

BOOST_AUTO_TEST_SUITE(Assets)

BOOST_AUTO_TEST_CASE(background_exists)
{
    BOOST_TEST(exists_file_not_empty(root() / "assets/img/world/background.png"));
}

BOOST_AUTO_TEST_CASE(floor_exists)
{
    BOOST_TEST(exists_file_not_empty(root() / "assets/img/world/floor.png"));
}

BOOST_AUTO_TEST_CASE(platform_exists)
{
    BOOST_TEST(exists_file_not_empty(root() / "assets/img/world/platform.png"));
}

BOOST_AUTO_TEST_CASE(lava_exists)
{
    BOOST_TEST(exists_file_not_empty(root() / "assets/img/world/lava.png"));
}

BOOST_AUTO_TEST_CASE(wallizq_exists)
{
    BOOST_TEST(exists_file_not_empty(root() / "assets/img/world/wallizq.png"));
}

BOOST_AUTO_TEST_CASE(wallder_exists)
{
    BOOST_TEST(exists_file_not_empty(root() / "assets/img/world/wallder.png"));
}

BOOST_AUTO_TEST_CASE(font_exists)
{
    BOOST_TEST(exists_file_not_empty(root() / "assets/fonts/ui.ttf"));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(PngValidity)

BOOST_AUTO_TEST_CASE(background_png_signature)
{
    auto p = root() / "assets/img/world/background.png";
    BOOST_REQUIRE(exists_file_not_empty(p));
    BOOST_TEST(has_png_signature(p));
}

BOOST_AUTO_TEST_CASE(platform_png_signature)
{
    auto p = root() / "assets/img/world/platform.png";
    BOOST_REQUIRE(exists_file_not_empty(p));
    BOOST_TEST(has_png_signature(p));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Locales)

BOOST_AUTO_TEST_CASE(es_mo_exists)
{
    BOOST_TEST(exists_file_not_empty(root() / "locale/es_ES/LC_MESSAGES/ProjectPrime8.mo"));
}

BOOST_AUTO_TEST_CASE(de_mo_exists)
{
    BOOST_TEST(exists_file_not_empty(root() / "locale/de_DE/LC_MESSAGES/ProjectPrime8.mo"));
}

BOOST_AUTO_TEST_CASE(fr_mo_exists)
{
    BOOST_TEST(exists_file_not_empty(root() / "locale/fr_FR/LC_MESSAGES/ProjectPrime8.mo"));
}

BOOST_AUTO_TEST_SUITE_END()
