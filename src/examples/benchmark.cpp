#include "dss/dss_isoiec9796-3.hpp"
#include "generic/zz_utils.hpp"

#include "algorithm/comb.hpp"
#include "algorithm/decomb.hpp"

#include <sys/time.h>
#include <list>
#include <utility>

using namespace NTL;

class NTLPRNG : public generateRandomValueCallback
{
    const ZZ _N;
public:
    NTLPRNG(const ZZ & Modulus)
        : _N(Modulus)
        {
            ZZ seed;
            seed += time(NULL);
            SetSeed(seed);
        }
    ~NTLPRNG() {}
    Octet getRandomValue()
        {
            return I2OSP(RandomBnd(_N));
        }
};

template<class DSS, class DSS_Field>
struct DefaultDSS
{
    const ECDataInputPolicy<DSS_Field>   DefaultInputPolicy;
    const DSSDomainParameters<DSS_Field> DomainParameters;
          generateRandomValueCallback &  PRNG;

    DefaultDSS(typename DSS_Field::aEC & Curve,
               const Algorithm::Precomputations_Method<typename DSS_Field::pECP, ZZ> & Precomputation,
               unsigned int L_rec,
               generateRandomValueCallback & PRNG)
        : DefaultInputPolicy(L_rec, Curve, Hash::RIPEMD160),
          DomainParameters(Curve, DefaultInputPolicy, Precomputation),
          PRNG(PRNG)
        {}


    DSS operator()()
        {
            return DSS(DomainParameters, PRNG);
        }
};


template<class DSS_Field>
struct DefaultDSS<ECKNR<DSS_Field>, DSS_Field>
{

    const ECDataInputPolicy<DSS_Field>   DefaultInputPolicy;
    const DSSDomainParameters<DSS_Field> DomainParameters;
          generateRandomValueCallback &  PRNG;

    DefaultDSS<ECKNR<DSS_Field>,
               DSS_Field>(typename DSS_Field::aEC & Curve,
                          const Algorithm::Precomputations_Method<typename DSS_Field::pECP, ZZ> & Precomputation,
                          unsigned int L_rec,
                          generateRandomValueCallback & PRNG)
    : DefaultInputPolicy(L_rec, Curve, Hash::RIPEMD160),
        DomainParameters(Curve, DefaultInputPolicy, Precomputation),
        PRNG(PRNG)
        {}

    ECKNR<DSS_Field> operator()()
        {
            return ECKNR<DSS_Field>(DomainParameters,
                                    DSSECKNRDomainParameters(Hash(Hash::RIPEMD160),
                                                             MGF(MGF::MGF2, Hash::RIPEMD160)),
                                    PRNG);
        }
};

template<class DSS_Field>
struct DefaultDSS<ECMR<DSS_Field>, DSS_Field>
{
    const ECMRDataInputPolicy<DSS_Field> DefaultInputPolicy;
    const DSSDomainParameters<DSS_Field> DomainParameters;
    const Hash                           _Hash;
    const MGF                            _MGF;
          generateRandomValueCallback &  PRNG;

    DefaultDSS<ECMR<DSS_Field>,
               DSS_Field>(typename DSS_Field::aEC & Curve,
                          const Algorithm::Precomputations_Method<typename DSS_Field::pECP, ZZ> & Precomputation,
                          unsigned int L_rec,
                          generateRandomValueCallback & PRNG)
    : DefaultInputPolicy(L_rec, Curve, Hash::SHA1),
        DomainParameters(Curve, DefaultInputPolicy, Precomputation),
        _Hash(Hash::SHA1),
        _MGF(MGF::MGF1,Hash::SHA1),
        PRNG(PRNG)
        {}

    ECMR<DSS_Field> operator()()
        {
            return ECMR<DSS_Field>(DomainParameters,
                                   DSSECMRDomainParameters(_Hash,
                                                           _MGF),
                                   PRNG);
        }
};

template<class DSS_Field>
struct DefaultDSS<ECAO<DSS_Field>, DSS_Field>
{
    const ECAODataInputPolicy<DSS_Field> DefaultInputPolicy;
    const DSSDomainParameters<DSS_Field> DomainParameters;
    const Hash                           _Hash;
    const MGF                            _MGF;
          generateRandomValueCallback &  PRNG;


    DefaultDSS<ECAO<DSS_Field>,
               DSS_Field>(typename DSS_Field::aEC & Curve,
                          const Algorithm::Precomputations_Method<typename DSS_Field::pECP, ZZ> & Precomputation,
                          unsigned int L_rec,
                          generateRandomValueCallback & PRNG)
    : DefaultInputPolicy(L_rec, 1, Curve, Hash::SHA256),
        DomainParameters(Curve, DefaultInputPolicy, Precomputation),
        _Hash(Hash::SHA256),
        _MGF(MGF::MGF1, Hash::SHA256),
        PRNG(PRNG) {}

    ECAO<DSS_Field> operator()()
        {
            return ECAO<DSS_Field>(DomainParameters,
                                   DSSECAODomainParameters(_Hash,
                                                           _MGF),
                                   PRNG);
        }
};

template<class DSS_Field>
struct DefaultDSS<ECPV<DSS_Field>, DSS_Field>
{
    const StaticDataInputPolicy          DefaultInputPolicy;
    const size_t                         KSize;
    const SymXor                         Sym;
    const DSSDomainParameters<DSS_Field> DomainParameters;
    const Hash                           _Hash;
    const DSSECPVDomainParameters        SchemeParameters;
          generateRandomValueCallback &  PRNG;

    DefaultDSS<ECPV<DSS_Field>,
               DSS_Field>(typename DSS_Field::aEC & Curve,
                          const Algorithm::Precomputations_Method<typename DSS_Field::pECP, ZZ> & Precomputation,
                          unsigned int L_rec,
                          generateRandomValueCallback & PRNG)
    : DefaultInputPolicy(13 - 2,
                         5  + 2,
                         13 + 5 + L_rec,
                         Hash::SHA1),
        KSize(NumBits(Curve.getOrder())),
        Sym(KSize),
      DomainParameters(Curve,
                       DefaultInputPolicy,
                       Precomputation),
        _Hash(Hash::SHA1),
        SchemeParameters(_Hash, Sym, KSize),
        PRNG(PRNG) {}

    ECPV<DSS_Field> operator()()
        {
            return ECPV<DSS_Field>(DomainParameters,
                                   SchemeParameters,
                                   PRNG);
        }
};

template <class DSS, typename DSS_Field>
unsigned int DSSBenchmark(typename DSS_Field::aEC & Curve,
                          const Octet & Xa,
                          generateRandomValueCallback & tG,
                          const Algorithm::Precomputations_Method<typename DSS_Field::pECP, ZZ> & Precomputation,
                          const unsigned int count)
{
    std::string M("This is a test message!");

    Curve.enter_mod_context(DSS_Field::aEC::MOD_CONTEXT::FIELD_CONTEXT);

    const unsigned long L_rec = 10;
    DefaultDSS<DSS, DSS_Field> dssBuilder(Curve, Precomputation, L_rec, tG);

    DSS dss = dssBuilder();

    dss.setPrivateKey(Xa);
    dss.generatePublicKey();
    dss.buildPrecomputationTables();

    struct timeval tv1, tv2;

    gettimeofday(&tv1, NULL);

    for (unsigned int i = 0; i < count; i++)
    {
        DigitalSignature sign = dss.sign(ManagedBlob(M.c_str(), M.length()));

        if (sign.R == sign.S) abort();
    }

    gettimeofday(&tv2, NULL);

    return tv2.tv_sec - tv1.tv_sec;
}

struct deCombBenchResults
{
    typedef std::pair<unsigned int,
                      unsigned int> deCombItem;

    long                  order;
    long                  count;
    unsigned int          Comb;
    std::list<deCombItem> deComb;

};

std::ostream& operator<<(std::ostream& s, const deCombBenchResults & results)
{
    s << results.order << " & " << results.Comb;
    for (std::list<deCombBenchResults::deCombItem>::const_iterator i = results.deComb.begin();
         i != results.deComb.end();
         i++)
    {
        s << " & ";

        unsigned int rval = (*i).second;

        if (rval == 0)
            s << " - ";
        else
            s << results.count/(double)rval;
    }
    return s;
}


template <class DSS, typename DSS_Field>
deCombBenchResults DSSBenchmarkWrapper(typename DSS_Field::aEC & Curve,
                                       const unsigned int count)
{
    NTLPRNG tG(Curve.getOrder());
    Octet Xa(tG());

    deCombBenchResults results;
    results.order = NumBits(Curve.getOrder());
    results.count = count;

    {
        const Algorithm::Precomputations_Method_Comb<
            typename DSS_Field::pECP,
            ZZ,
            typename DSS_Field::aECP> Precomputation (NumBits(Curve.getModulus()));

        results.Comb = DSSBenchmark<DSS, DSS_Field>(Curve, Xa, tG, Precomputation, count);
    }

    for (int i = 0; i<NumBits(Curve.getOrder())/8/2; i ++)
    {
        typename Algorithm::Precomputations_deComb<
            typename DSS_Field::pECP,
            typename DSS_Field::aECP>::Generator dtG (8,i,NumBits(Curve.getModulus()), tG);

        const Algorithm::Precomputations_Method_deComb<
            typename DSS_Field::pECP,
            typename DSS_Field::aECP> Precomputation (dtG);

        results.deComb.push_back(
            deCombBenchResults::deCombItem(i, DSSBenchmark<DSS, DSS_Field>(Curve, Xa, dtG, Precomputation, count)));
    }

    return results;
}

std::ostream & operator<<(std::ostream & s, std::list<deCombBenchResults> & results_list)
{
    std::list<std::string> results;

    for (std::list<deCombBenchResults>::iterator i = results_list.begin();
         i != results_list.end();
         i ++)
    {
        std::stringstream result (std::stringstream::in | std::stringstream::out);
        result << *i;
        results.push_back(result.str());
    }

    // Find maximal fields count

    unsigned int maximal_fields_count = 0;

    for (std::list<std::string>::iterator i = results.begin();
         i != results.end();
         i ++)
    {
        unsigned int this_fields_count = 0;
        for (std::string::iterator j = (*i).begin();
             j != (*i).end();
             j ++)
            if (*j == '&')
                this_fields_count ++;

        maximal_fields_count = std::max(maximal_fields_count, this_fields_count);
    }

    // Output table specification
    s << "\\begin{tabular}{|r|l|";

    for (unsigned int i = 0; i < maximal_fields_count - 2; i ++)
        s << "l|";
    s << "}" << std::endl;

    // Output table header
    s << "$\\log_2n$ & Comb";
    for (unsigned int i = 0; i < maximal_fields_count - 2; i ++)
        s << " & $\\epsilon=" << i << "$";
    s << "\\\\\\hline" << std::endl;

    // Add missing fields and output them with closed lines
    for (std::list<std::string>::iterator i = results.begin();
         i != results.end();
         i ++)
    {
        unsigned int add_fields_count = maximal_fields_count;

        for (std::string::iterator j = (*i).begin();
             j != (*i).end();
             j ++)
            if (*j == '&')
                add_fields_count --;

        s << *i;

        for (unsigned int i = 0; i < add_fields_count; i++)
            s << " & -";

        s << "\\\\\\hline" << std::endl;
    }

    // Close table specification
    s << "\\end{tabular}";
    return s;
}


template <class DSS, typename DSS_Field>
void MakeGFPTests(const char * caption, int count)
{
    using namespace ECZZ_p::Affine;
    std::list<EC_Defaults::Sizes> AffineSizes;

    AffineSizes.push_back(EC_Defaults::Sizes::EC160);
    AffineSizes.push_back(EC_Defaults::Sizes::EC161);
    AffineSizes.push_back(EC_Defaults::Sizes::EC192);

    std::list<deCombBenchResults> results;

    std::cout << "\\begin{table}[!hb]" << std::endl
              << "\\centering"         << std::endl;

    for (std::list<EC_Defaults::Sizes>::iterator i = AffineSizes.begin();
         i != AffineSizes.end();
         i ++)
    {
        EC Curve(EC_Defaults::create(*i));
        results.push_back(DSSBenchmarkWrapper<DSS, DSS_Field>(Curve, count));
    }

    std::cout << results << std::endl
              << "\\caption{" << caption << "}" << std::endl
              << "\\end{table}" << std::endl;

}

template <class DSS, typename DSS_Field>
void MakeGF2XTests(const char * caption, int count)
{
    using namespace ECGF2X::Affine;
    std::list<EC_Defaults::Sizes> AffineSizes;

    AffineSizes.push_back(EC_Defaults::Sizes::EC163);
    AffineSizes.push_back(EC_Defaults::Sizes::EC283);
    AffineSizes.push_back(EC_Defaults::Sizes::EC571);

    std::list<deCombBenchResults> results;

    std::cout << "\\begin{table}[!hb]" << std::endl
              << "\\centering"         << std::endl;

    for (std::list<EC_Defaults::Sizes>::iterator i = AffineSizes.begin();
         i != AffineSizes.end();
         i ++)
    {
        EC Curve(EC_Defaults::create(*i));
        results.push_back(DSSBenchmarkWrapper<DSS, DSS_Field>(Curve, count));
    }

    std::cout << results << std::endl
              << "\\caption{" << caption << "}" << std::endl
              << "\\end{table}" << std::endl;
}


int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " [1-2^32]" << std::endl;
        return 1;
    }

    unsigned int count = atoi(argv[1]);

    MakeGFPTests<ECNR<DSS_ZZ_p>,  DSS_ZZ_p>("ECNR, $GF(P)$",   count);
    MakeGFPTests<ECMR<DSS_ZZ_p>,  DSS_ZZ_p>("ECMR, $GF(P)$",   count);
    MakeGFPTests<ECAO<DSS_ZZ_p>,  DSS_ZZ_p>("ECAO, $GF(P)$",   count);
    MakeGFPTests<ECPV<DSS_ZZ_p>,  DSS_ZZ_p>("ECPV, $GF(P)$",   count);
    MakeGFPTests<ECKNR<DSS_ZZ_p>, DSS_ZZ_p>("ECKNR, $GF(P)$",  count);

    MakeGF2XTests<ECNR<DSS_GF2X>,  DSS_GF2X>("ECNR, $GF(2^m)$", count);
    MakeGF2XTests<ECMR<DSS_GF2X>,  DSS_GF2X>("ECMR, $GF(2^m)$", count);
    MakeGF2XTests<ECAO<DSS_GF2X>,  DSS_GF2X>("ECAO, $GF(2^m)$", count);
    MakeGF2XTests<ECPV<DSS_GF2X>,  DSS_GF2X>("ECPV, $GF(2^m)$", count);
    MakeGF2XTests<ECKNR<DSS_GF2X>, DSS_GF2X>("ECKNR, $GF(2^m)$",count);

    return 0;
}
