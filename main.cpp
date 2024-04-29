#include "num.hpp"
static Num randNum()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr;

    Num randomNum = distr(gen);
    return randomNum;
}

static Num generatePrime(int bitLength)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr;

    Num num;
    bool isPrime;
    do
    {
        num = distr(gen);
        num = num % (Num(1) << bitLength);
        isPrime = true;
        for (Num i = 2; i <= num.sqrt(); i += 1)
        {
            if (num % i == 0)
            {
                isPrime = false;
                break;
            }
        }
    } while (!isPrime);
    return num;
}
static std::string toString(const Num &num,
                            uint64_t base = 26, const char *alphabet = "abcdefghijklmnopqrstuvwxyz")
{
    std::vector<char> text;

    num.print(text, base, alphabet);
    text.pop_back();
    return std::string(text.begin(), text.end());
}

static Num mod_pow(Num a, Num exponent, const Num &modulus)
{
    Num result(1), base = a % modulus;
    for (; exponent.size() > 0; exponent >>= 1)
    {
        if (exponent.get_bit(0))
        {
            result = (result * base) % modulus;
        }
        base = (base * base) % modulus;
    }
    return result;
}

static Num fromString(const std::string &str,
                      uint64_t base = 26, const char *alphabet = "abcdefghijklmnopqrstuvwxyz")
{
    Num result;
    for (char c : str)
    {
        int digit = -1;
        for (int i = 0; i < base; ++i)
        {
            if (c == alphabet[i])
            {
                digit = i;
                break;
            }
        }
        if (digit == -1)
        {
            throw std::invalid_argument("Invalid character in input string");
        }
        result *= base;
        result += digit;
    }
    return result;
}

static Num modInverse(Num t, const Num &p)
{
    Num a = t;
    Num m = p;
    Num m0 = m;
    Num y = 0, x = 1;

    if (m == 1)
    {
        return 0;
    }

    while (a > 1)
    {
        Num q = a / m;
        Num t = m;

        m = a % m, a = t;
        t = y;

        y = x - q * y;
        x = t;
    }

    if (x < 0)
    {
        x += m0;
    }

    return x;
}

int main()
{
    Num p = generatePrime(512);
    Num q = generatePrime(512);
    Num n = p * q;
    Num phi = (p - 1) * (q - 1);

    Num e;
    do
    {
        e = randNum() % (phi - 1) + 1;
    } while (Num::gcd(e, phi) != 1);
    Num d = modInverse(e, phi);

    Num r;
    do
    {
        r = randNum();
    } while (Num::gcd(r, n) != 1);

    std::string msg = "helloworldasd";
    std::cout << "Message: " << msg << std::endl;

    Num m = fromString(msg);

    Num m_1 = m * mod_pow(r, e, n); // Blinding the message

    Num s_1 = mod_pow(m_1, d, n); // Signing the blinded message

    Num s = s_1 * modInverse(r, n); // Unblinding the signature

    Num verification = mod_pow(s, e, n); // Verifying the signature

    std::cout << "Verification: " << toString(verification) << std::endl;
    if (verification == m)
    {
        std::cout << "Signature is valid!" << std::endl;
    }
    else
    {
        std::cout << "Signature is invalid!" << std::endl;
    }

    return 0;
}