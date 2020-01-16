//
// Created by liang on 1/16/20.
//

#include "Greeting.h"

// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "eEVM/opcode.h"
#include "eEVM/processor.h"
#include "eEVM/simple/simpleglobalstate.h"

#include "eEVM/fmt/format_header_only.h"

#include <stdio.h>

std::vector<uint8_t> create_bytecode(const std::string& s)
{
    std::vector<uint8_t> code;
    constexpr uint8_t mdest = 0x0;
    const uint8_t rsize = s.size() + 1;

    // Store each byte in evm memory
    uint8_t mcurrent = mdest;
    for (const char& c : s)
    {
        code.push_back(eevm::Opcode::PUSH1);
        code.push_back(c);
        code.push_back(eevm::Opcode::PUSH1);
        code.push_back(mcurrent++);
        code.push_back(eevm::Opcode::MSTORE8);
    }

    // Return
    code.push_back(eevm::Opcode::PUSH1);
    code.push_back(rsize);
    code.push_back(eevm::Opcode::PUSH1);
    code.push_back(mdest);
    code.push_back(eevm::Opcode::RETURN);

    return code;
}

void sayHello()
{
    // Create random addresses for sender and contract
    std::vector<uint8_t> raw_address(20);
    std::generate(
            raw_address.begin(), raw_address.end(), []() { return rand(); });
    const eevm::Address sender =
            eevm::from_big_endian(raw_address.data(), raw_address.size());

    std::generate(
            raw_address.begin(), raw_address.end(), []() { return rand(); });
    const eevm::Address to =
            eevm::from_big_endian(raw_address.data(), raw_address.size());

    // Create global state
    eevm::SimpleGlobalState gs;

    // Create code
    std::string hello_world("Hello world!");
    const eevm::Code code = create_bytecode(hello_world);

    // Deploy contract to global state
    const eevm::AccountState contract = gs.create(to, 0, code);

    // Create transaction
    eevm::NullLogHandler ignore;
    eevm::Transaction tx(sender, ignore);

    // Create processor
    eevm::Processor p(gs);

    // Execute code. All execution is associated with a transaction. This
    // transaction is called by sender, executing the code in contract, with empty
    // input (and no trace collection)
    const eevm::ExecResult e = p.run(tx, sender, contract, {}, 0, nullptr);



    // Create string from response data, and print it
    const std::string response(reinterpret_cast<const char*>(e.output.data()));


    fprintf(stdout, "Hello world from Greeting\n");
    fprintf(stdout, response.c_str());
    fprintf(stdout, "\nHello world from Greeting2\n");

    //std::cout << response << std::endl;


}
