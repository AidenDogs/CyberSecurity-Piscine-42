#ifndef HOTP_HPP
# define HOTP_HPP

# include "common.hpp"
# include "crypto.hpp"
# include "hex.hpp"
# include "env.hpp"

void generateOTP(const std::string& keyFilePath);

#endif