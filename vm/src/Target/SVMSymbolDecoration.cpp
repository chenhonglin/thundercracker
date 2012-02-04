/*
 * This file is part of the Sifteo VM (SVM) Target for LLVM
 *
 * M. Elizabeth Scott <beth@sifteo.com>
 * Copyright <c> 2012 Sifteo, Inc. All rights reserved.
 */

#include "SVMSymbolDecoration.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Module.h"
#include "llvm/Constant.h"
#include "llvm/GlobalValue.h"
#include "llvm/GlobalAlias.h"
#include "llvm/Type.h"
#include "llvm/DerivedTypes.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCValue.h"
#include "llvm/MC/MCExpr.h"
using namespace llvm;

// Prefixes that are intended for use in source code
const char SVMDecorations::SYS[] = "_SYS_";
        
// For internal use
const char SVMDecorations::CALL[] = "_call$";
const char SVMDecorations::TCALL[] = "_tcall$";

const char *SVMEntryPoint::nameTable[] = {
    "main",                 // Main C name
    "siftmain",             // Backwards-compatible C name
    "_Z4mainv",             // Main C++ mangled name
    "_Z8siftmainv",         // Backwards-compatible C++ mangled name
    NULL,
};

StringRef SVMEntryPoint::getPreferredSignature()
{
    return "void main()";
}

bool SVMEntryPoint::isEntry(StringRef Name)
{
    for (unsigned i = 0; nameTable[i]; i++) {
        StringRef candidate = nameTable[i];
        if (candidate == Name)
            return true;
    }
    return false;
}

MCSymbol *SVMEntryPoint::findEntry(const MCContext &Ctx)
{
    for (unsigned i = 0; nameTable[i]; i++) {
        MCSymbol *Entry = Ctx.LookupSymbol(nameTable[i]);
        if (Entry && Entry->isDefined())
            return Entry;
    }
    return NULL;
}

Constant *SVMDecorations::Apply(Module *M, const GlobalValue *Value, StringRef Prefix)
{
    /*
     * Get or create a Constant that represents a decorated version of
     * the given GlobalValue. The decorated symbol is 'Prefix' concatenated
     * with the original symbol value.
     */

    GlobalValue *GV = (GlobalValue*) Value;
    Twine Name = Twine(Prefix) + GV->getName();
    GlobalAlias *GA = M->getNamedAlias(Name.str());

    if (!GA) {
        GA = new GlobalAlias(GV->getType(), GlobalValue::ExternalLinkage, Name, GV, M);
        GA->copyAttributesFrom(GV);
    }

    return GA;
}

StringRef SVMDecorations::Decode(StringRef Name)
{
    // Internal prefixes are always stripped
    isTailCall = testAndStripPrefix(Name, TCALL);
    isCall = isTailCall || testAndStripPrefix(Name, CALL);

    // XXX: Not sure why, but Clang is prepending this junk to __asm__ symbols
    testAndStripPrefix(Name, "\x01");
    testAndStripPrefix(Name, "_01_");

    /*
     * At this point, we have the full user-specified name of either
     * a normal or special symbol. This is always the value we return,
     * even if further results are decoded into the SVMDecorations struct.
     */
    StringRef Result = Name;

    // Numeric syscalls can be written in any base, using C-style numbers
    isSys = testAndStripPrefix(Name, SYS) && !Name.getAsInteger(0, sysNumber);

    return Result;
}

bool SVMDecorations::testAndStripPrefix(StringRef &Name, StringRef Prefix)
{
    if (Name.startswith(Prefix)) {
        Name = Name.substr(Prefix.size());
        return true;
    }
    return false;
}
