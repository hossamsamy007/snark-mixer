#ifndef __ZKSNARK_COMPUTATION_TCC__
#define __ZKSNARK_COMPUTATION_TCC__

#include <cstdlib>
#include <libsnark_helpers/libsnark_helpers.hpp>

template<typename FieldT>
void generate_proof(libsnark::protoboard<FieldT> pb) {
    // See: https://github.com/scipr-lab/libsnark/blob/92a80f74727091fdc40e6021dc42e9f6b67d5176/libsnark/relations/constraint_satisfaction_problems/r1cs/r1cs.hpp#L81
    // For the definition of r1cs_primary_input and r1cs_auxiliary_input
    libsnark::r1cs_primary_input<FieldT> primary_input = pb.primary_input(); // Public input
    libsnark::r1cs_auxiliary_input<FieldT> auxiliary_input = pb.auxiliary_input(); // Private input (Known only by the prover)

    // Retrieve proving key from file
    boost::filesystem::path setup_dir = getPathToSetupDir();
    boost::filesystem::path prov_key_raw("pk.raw");
    boost::filesystem::path full_path_prov_key_raw = setup_dir / prov_key_raw;
    libsnark::r1cs_ppzksnark_proving_key<libff::alt_bn128_pp> proving_key = deserializeProvingKeyFromFile(full_path_prov_key_raw);

    // Generate proof from public input, auxiliary input (private/secret data), and proving key
    libsnark::r1cs_ppzksnark_proof<libff::alt_bn128_pp> proof = libsnark::r1cs_ppzksnark_prover<libff::alt_bn128_pp>(proving_key, primary_input, auxiliary_input);

    // Write proof to json file
    proof_to_json(proof, primary_input);
}

template<typename FieldT>
void run_trusted_setup(libsnark::protoboard<FieldT> pb) {
    boost::filesystem::path setup_dir = getPathToSetupDir();
    boost::filesystem::path verif_key_json("vk.json");
    boost::filesystem::path verif_key_raw("vk.raw");
    boost::filesystem::path prov_key_raw("pk.raw");
    boost::filesystem::path full_path_verif_key_json = setup_dir / verif_key_json;
    boost::filesystem::path full_path_verif_key_raw = setup_dir / verif_key_raw;
    boost::filesystem::path full_path_prov_key_raw = setup_dir / prov_key_raw;

    // Generate verification key and proving key (Trusted setup) from the defined R1CS (defined in the ZoKrates/wraplibsnark.cpp file)
    libsnark::r1cs_ppzksnark_keypair<libff::alt_bn128_pp> keypair = generateKeypair(pb.get_constraint_system());

    // Write the verification key in JSON format to later use in the Verifier contract
    verificationKey_to_json(keypair, full_path_verif_key_json);

    // Write proving key and verification key in raw format (defined in the ZoKrates/wraplibsnark.cpp file)
    serializeProvingKeyToFile(keypair.pk, full_path_prov_key_raw);
    serializeVerificationKeyToFile(keypair.vk, full_path_verif_key_raw);
}

#endif
