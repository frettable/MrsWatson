//
// SampleBufferTest.c - MrsWatson
// Copyright (c) 2016 Teragon Audio. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//

#include "audio/SampleBuffer.h"

#include "audio/AudioSettings.h"
#include "unit/TestRunner.h"

static SampleBuffer _newMockSampleBuffer(void) { return newSampleBuffer(1, 1); }

static int _testNewSampleBuffer(void) {
  SampleBuffer s = _newMockSampleBuffer();
  assertIntEquals(1, s->numChannels);
  assertUnsignedLongEquals(1l, s->blocksize);
  freeSampleBuffer(s);
  return 0;
}

static int _testNewSampleBufferMultichannel(void) {
  SampleBuffer s = newSampleBuffer(8, 128);
  unsigned int i, j;
  assertNotNull(s);
  assertIntEquals(8, s->numChannels);

  // Actually write a bunch of samples to expose memory corruption
  for (i = 0; i < s->blocksize; ++i) {
    for (j = 0; j < s->numChannels; ++j) {
      s->samples[j][i] = 0.5f;
    }
  }

  freeSampleBuffer(s);
  return 0;
}

static int _testClearSampleBuffer(void) {
  SampleBuffer s = _newMockSampleBuffer();
  s->samples[0][0] = 123;
  sampleBufferClear(s);
  assertDoubleEquals(0.0, s->samples[0][0], TEST_DEFAULT_TOLERANCE);
  freeSampleBuffer(s);
  return 0;
}

static int _testCopyAndMapChannelsSampleBuffers(void) {
  SampleBuffer s1 = _newMockSampleBuffer();
  SampleBuffer s2 = _newMockSampleBuffer();
  s1->samples[0][0] = 123.0;
  assert(sampleBufferCopyAndMapChannels(s2, s1));
  assertDoubleEquals(123.0, s2->samples[0][0], TEST_DEFAULT_TOLERANCE);
  freeSampleBuffer(s1);
  freeSampleBuffer(s2);
  return 0;
}

static int _testCopyAndMapChannelsSampleBuffersDifferentBlocksizes(void) {
  SampleBuffer s1 = newSampleBuffer(1, DEFAULT_BLOCKSIZE);
  SampleBuffer s2 = _newMockSampleBuffer();

  s1->samples[0][0] = 123.0;
  assertFalse(sampleBufferCopyAndMapChannels(s2, s1));
  // Contents should not change; copying with different sizes is invalid
  assertDoubleEquals(123.0, s1->samples[0][0], TEST_DEFAULT_TOLERANCE);

  freeSampleBuffer(s1);
  freeSampleBuffer(s2);
  return 0;
}

static int _testCopyAndMapChannelsSampleBuffersDifferentChannelsBigger(void) {
  SampleBuffer s1 = newSampleBuffer(4, 1);
  SampleBuffer s2 = newSampleBuffer(2, 1);

  s2->samples[0][0] = 1.0;
  s2->samples[1][0] = 2.0;

  assert(sampleBufferCopyAndMapChannels(s1, s2));
  assertDoubleEquals(1.0, s1->samples[0][0], TEST_DEFAULT_TOLERANCE);
  assertDoubleEquals(2.0, s1->samples[1][0], TEST_DEFAULT_TOLERANCE);
  assertDoubleEquals(1.0, s1->samples[2][0], TEST_DEFAULT_TOLERANCE);
  assertDoubleEquals(2.0, s1->samples[3][0], TEST_DEFAULT_TOLERANCE);

  freeSampleBuffer(s1);
  freeSampleBuffer(s2);
  return 0;
}

static int _testCopyAndMapChannelsSampleBuffersDifferentChannelsSmaller(void) {
  SampleBuffer s1 = newSampleBuffer(1, 1);
  SampleBuffer s2 = newSampleBuffer(4, 1);
  unsigned int i;

  for (i = 0; i < s1->numChannels; i++) {
    s1->samples[i][0] = 1.0;
  }

  for (i = 0; i < s2->numChannels; i++) {
    s2->samples[i][0] = 2.0;
  }

  assert(sampleBufferCopyAndMapChannels(s1, s2));
  assertDoubleEquals(2.0, s1->samples[0][0], TEST_DEFAULT_TOLERANCE);

  freeSampleBuffer(s1);
  freeSampleBuffer(s2);
  return 0;
}

static int _testFreeNullSampleBuffer(void) {
  freeSampleBuffer(NULL);
  return 0;
}

TestSuite addSampleBufferTests(void);
TestSuite addSampleBufferTests(void) {
  TestSuite testSuite = newTestSuite("SampleBuffer", NULL, NULL);
  addTest(testSuite, "NewObject", _testNewSampleBuffer);
  addTest(testSuite, "NewSampleBufferMultichannel",
          _testNewSampleBufferMultichannel);
  addTest(testSuite, "ClearSampleBuffer", _testClearSampleBuffer);
  addTest(testSuite, "CopyAndMapChannelsSampleBuffers",
          _testCopyAndMapChannelsSampleBuffers);
  addTest(testSuite, "CopyAndMapChannelsSampleBuffersDifferentSizes",
          _testCopyAndMapChannelsSampleBuffersDifferentBlocksizes);
  addTest(testSuite, "CopyAndMapChannelsSampleBuffersDifferentChannelsBigger",
          _testCopyAndMapChannelsSampleBuffersDifferentChannelsBigger);
  addTest(testSuite, "CopyAndMapChannelsSampleBuffersDifferentChannelsSmaller",
          _testCopyAndMapChannelsSampleBuffersDifferentChannelsSmaller);
  addTest(testSuite, "FreeNullSampleBuffer", _testFreeNullSampleBuffer);
  return testSuite;
}
