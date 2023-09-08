/**
 * @file
 */
#ifndef CONFIG_STREAMBUF_HPP
#define CONFIG_STREAMBUF_HPP

#include <map>
#include <streambuf>
#include <string>
#include <vector>


namespace configure {

/**
 * Filtering input buffer to perform parameter substitution.
 */
class StreamBuffer: public std::streambuf {
public:
    /**
     *
     * @param source source buffer
     * @param params mapping of parameter substitutions
     */
    explicit StreamBuffer(std::streambuf* source, const std::map<std::string, std::string>& = {});

protected:
    using std::streambuf::int_type;

    /**
     * Called when the buffer is is empty.
     *
     * This will pull additional characters from the source buffer and perform
     * parameter substitution.
     *
     * @return first new character
     */
    int_type underflow() override;

private:
    static constexpr int_type eof{traits_type::eof()};
    static constexpr std::vector<char>::size_type buflen{1024};
    const std::map<std::string, std::string> params;
    std::streambuf* source;
    std::vector<char> buffer;
};

}  // namespace config


#endif  // CONFIG_STREAMBUF_HPP
