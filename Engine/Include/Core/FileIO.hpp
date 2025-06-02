#pragma once

namespace FS
{
    enum class Location
    {
        eEngine,
        eProject,
    };
    class FileIO 
    {
    public:
        [[nodiscard]] static std::string GetPath(Location directory, std::string_view path);
        
        /// <summary>
        /// Read a text file into a string. The string is empty if the file was not found.
        /// </summary>
        [[nodiscard]] static std::string ReadTextFile(std::string_view path);

        /// <summary>
        /// Write a string to a text file. The file is created if it does not exist.
        /// Returns true if the file was written successfully.
        /// </summary>
        [[nodiscard]] static bool WriteTextFile(std::string_view path, std::string_view content);

        /// <summary>
        /// Read a binary file into a vector of chars. The vector is empty if the file was not found.
        /// </summary>
        [[nodiscard]] static std::vector<char> ReadBinaryFile(std::string_view path);

        /// <summary>
        /// Write a string to a binary file. The file is created if it does not exist.
        /// Returns true if the file was written successfully.
        /// </summary>
        [[nodiscard]] static bool
        WriteBinaryFile(std::string_view path, const std::vector<char>& content);

        /// <summary>
        /// Check if a file exists.
        /// </summary>
        [[nodiscard]] static bool Exists(std::string_view path);

        /// <summary>
        /// Check the last time a file was modified.
        /// </summary>
        [[nodiscard]] static uint64_t LastModified(std::string_view path);
    };
} // namespace FS