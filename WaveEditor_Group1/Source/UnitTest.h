/*
  ==============================================================================

    UnitTest.h
    Created: 28 Apr 2020 9:18:50pm
    Author:  Yilin Zhang

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Utils.h"

class KoolEditTest  : public UnitTest
{
public:
    KoolEditTest()  : UnitTest ("KoolEdit Test") {}

    void runTest() override
    {
        AudioBuffer<float> testBuffer;

        beginTest ("AudioBufferUtilsTest");

        ////////// Test deleteRegion
        testBuffer.setSize(1, 10);
        AudioBufferUtils<float>::deleteRegion(testBuffer, 2, 4);
        expectEquals(testBuffer.getNumSamples(), 6, "deleteRegion failed.");

        ////////// Test insertRegion
        // prepare the buffer to be inserted
        testBuffer.clear();
        testBuffer.setSize(1, 10);
        auto writePointer = testBuffer.getWritePointer(0);
        for (int i=0; i<testBuffer.getNumSamples(); i++)
            writePointer[i] = 0;
        // prepare insert buffer
        AudioBuffer<float> insertBuffer {1, 5};
        writePointer = insertBuffer.getWritePointer(0);
        for (int i=0; i<testBuffer.getNumSamples(); i++)
            writePointer[i] = 1;
        // prepare ground truth buffer
        AudioBuffer<float> trueBuffer {1, 10+5};
        writePointer = trueBuffer.getWritePointer(0);
        for (int i=0; i<5; i++)
            writePointer[i] = 0;
        for (int i=5; i<10; i++)
            writePointer[i] = 1;
        for (int i=10; i<15; i++)
            writePointer[i] = 0;
        // operate
        AudioBufferUtils<float>::insertRegion(testBuffer, insertBuffer, 5);
        auto testBufferWritePointer = testBuffer.getWritePointer(0);
        auto trueBufferWritePointer = trueBuffer.getWritePointer(0);
        for (int i=0; i<15; i++)
            expectEquals(testBufferWritePointer[i], trueBufferWritePointer[i], "insertRegion failed.");
    }
};

// Creating a static instance will automatically add the instance to the array
// returned by UnitTest::getAllTests(), so the test will be included when you call
// UnitTestRunner::runAllTests()
static KoolEditTest test;
