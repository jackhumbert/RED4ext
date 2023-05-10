#include <Paths.hpp>
#include <Systems/ScriptCompilationSystem.hpp>
#include <catch2/catch_test_macros.hpp>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <v0/Plugin.hpp>


class TestPlugin : public PluginBase
{
public:
    TestPlugin(const std::filesystem::path& aPath)
        : PluginBase(aPath, nullptr)
        , semVer(RED4EXT_SEMVER(0, 0, 0))
        , fileVer(RED4ext::FileVer(0, 0, 0, 0))
    {
    }

    virtual const std::wstring_view GetName() const
    {
        return L"Test Plugin";
    }

    virtual const uint32_t GetApiVersion() const
    {
        return 0;
    }

    virtual void* GetPluginInfo()
    {
        return nullptr;
    }

    virtual const void* GetSdkStruct() const
    {
        return nullptr;
    }

    virtual const std::wstring_view GetAuthor() const
    {
        return L"";
    }

    virtual const RED4ext::SemVer& GetVersion() const
    {
        return semVer;
    }

    virtual const RED4ext::FileVer& GetRuntimeVersion() const
    {
        return fileVer;
    }

    virtual const RED4ext::SemVer& GetSdkVersion() const
    {
        return semVer;
    }

    RED4ext::SemVer semVer;
    RED4ext::FileVer fileVer;
};

TEST_CASE("Script Compilation System", "[redscript]")
{
    auto paths = new Paths();
    std::wstring fileName;
    wil::GetModuleFileNameW(nullptr, fileName);
    auto logger = spdlog::rotating_logger_mt("RED4ext Tests", (std::filesystem::path)fileName.append(L"-tests.log"),
                                             1024 * 1024, 0, false);
    logger->set_level(spdlog::level::info);
    logger->flush_on(spdlog::level::info);
    spdlog::set_default_logger(logger);
    auto scs = new ScriptCompilationSystem(*paths);
    FixedWString og;
    og.str = L"Test";
    GIVEN("a script is added from a plugin")
    {
        std::wofstream pathsFile(L"script.reds", std::ios::out);
        pathsFile << "public class TestClass { }" << std::endl;
        TestPlugin plugin(fileName);
        auto handle = std::shared_ptr<PluginBase>(&plugin);
        REQUIRE(scs->Add(handle, L"script.reds"));

        WHEN("scriptsBlobPath is provided")
        {
            scs->SetScriptsBlob("C:/Test.redscripts");
            auto args = scs->GetCompilationArgs(og);
            REQUIRE(!args.empty());

            THEN("the arg string matches the expected value")
            {
                auto expected = fmt::format(LR"(-compile "{}" "C:/Test.redscripts" -compilePathsFile "{}")",
                                            paths->GetR6Scripts().c_str(), paths->GetRedscriptPathsFile().c_str());
                wprintf(L"%s\n", args.c_str());
                REQUIRE(args == expected);
            }
        }

        WHEN("no scriptsBlobPath is provided")
        {
            auto args = scs->GetCompilationArgs(og);
            REQUIRE(!args.empty());

            THEN("the arg string matches the expected value")
            {
                auto expected = fmt::format(LR"(Test -compilePathsFile "{}")", paths->GetRedscriptPathsFile().c_str());
                wprintf(L"%s\n", args.c_str());
                REQUIRE(args == expected);
            }
        }
    }
    spdlog::details::registry::instance().flush_all();
    spdlog::shutdown();
    paths->~Paths();
}
