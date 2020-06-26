#!/usr/bin/env python

import datetime
import os
import sys

OutputFile = os.path.abspath(sys.argv[1])
Now = datetime.datetime.now()

Prefix = """//
// Copyright (c) 2020-{0} Nicholas Corgan
// SPDX-License-Identifier: GPL-3.0-or-later
//
// This file was generated on {1}.
//""".format(Now.year, Now)

StandardNames = [
    "GSM XCCH",
    "GPRS CS2",
    "GPRS CS3",
    "GSM RACH",
    "GSM SCH",
    "GSM TCH-FR",
    "GSM TCH-HR",
    "GSM TCH-AFS12.2",
    "GSM TCH-AFS10.2",
    "GSM TCH-AFS7.95",
    "GSM TCH-AFS7.4",
    "GSM TCH-AFS6.7",
    "GSM TCH-AFS5.9",
    "GSM TCH-AHS7.95",
    "GSM TCH-AHS7.4",
    "GSM TCH-AHS6.7",
    "GSM TCH-AHS5.9",
    "GSM TCH-AHS5.15",
    "GSM TCH-AHS4.75",
    "WiMax FCH",
    "LTE PBCH",
]

# Note: we need the dummy functions, or Pothos will parse everything as a single
# giant doc.
EncoderTemplate = """
/*
 * |PothosDoc {0} Encoder
 *
 * Takes in a bytestream of {0} data and outputs the bytestream, encoded.
 * The convolution parameters used for this encoding are standard-specific
 * and are read-only.
 *
 * |category /FEC/{1}
 * |keywords coder lte
 * |factory /fec/{2}_encoder()
 */
void {2}();
"""
DecoderTemplate = """
/*
 * |PothosDoc {0} Decoder
 *
 * Takes in a bytestream of encoded {0} data and outputs the bytestream, decoded.
 * The convolution parameters used for this encoding are standard-specific
 * and are read-only.
 *
 * |category /FEC/{1}
 * |keywords coder lte
 * |factory /fec/{2}_decoder()
 */
void {2}();
"""

def convertStandardName(standardName):
    convertedStandardName = standardName.lower()
    for c in [" ","-","."]:
        convertedStandardName = convertedStandardName.replace(c,"_")

    return convertedStandardName

def outputDocFile():
    encoderDocs = "".join([EncoderTemplate.format(standardName, standardName.split(" ")[0], convertStandardName(standardName)) for standardName in StandardNames])
    decoderDocs = "".join([DecoderTemplate.format(standardName, standardName.split(" ")[0], convertStandardName(standardName)) for standardName in StandardNames])
    fileContents = "{0}\n{1}\n{2}".format(Prefix, encoderDocs, decoderDocs)

    with open(OutputFile, "w") as f:
        f.write(fileContents)

if __name__ == "__main__":
    outputDocFile()
