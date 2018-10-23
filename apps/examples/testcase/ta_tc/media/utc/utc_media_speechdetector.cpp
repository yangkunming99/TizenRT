/****************************************************************************
 *
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/

#include <media/voice/SpeechDetector.h>

#include "tc_common.h"

#define TEST_SAMPLE_RATE 16000
#define TEST_CHANNELS 2

static void utc_media_SpeechDetector_instance_p(void)
{
	auto p1 = media::voice::SpeechDetector::instance();
	auto p2 = media::voice::SpeechDetector::instance();
	TC_ASSERT_EQ("instance", p1, p2);
	TC_SUCCESS_RESULT();
}

static void utc_media_SpeechDetector_initKeywordDetect_p(void)
{
	auto instance = media::voice::SpeechDetector::instance();
	bool ret = instance->initKeywordDetect(TEST_SAMPLE_RATE, TEST_CHANNELS);
	TC_ASSERT_EQ("initKeywordDetect", ret, true);
	TC_SUCCESS_RESULT();
	instance->deinitKeywordDetect();
}

static void utc_media_SpeechDetector_initKeywordDetect_n(void)
{
	auto instance = media::voice::SpeechDetector::instance();
	bool ret = instance->initKeywordDetect(0, TEST_CHANNELS);
	TC_ASSERT_EQ("initKeywordDetect", ret, false);
	TC_SUCCESS_RESULT();
}

static void utc_media_SpeechDetector_initEndPointDetect_p(void)
{
	auto instance = media::voice::SpeechDetector::instance();
	bool ret = instance->initEndPointDetect(TEST_SAMPLE_RATE, TEST_CHANNELS);
	TC_ASSERT_EQ("initEndPointDetect", ret, true);
	TC_SUCCESS_RESULT();
	instance->deinitEndPointDetect();
}

static void utc_media_SpeechDetector_initEndPointDetect_n(void)
{
	auto instance = media::voice::SpeechDetector::instance();
	bool ret = instance->initEndPointDetect(0, TEST_CHANNELS);
	TC_ASSERT_EQ("initEndPointDetect", ret, false);
	TC_SUCCESS_RESULT();
}

static void utc_media_SpeechDetector_deinitKeywordDetect_p(void)
{
	auto instance = media::voice::SpeechDetector::instance();
	instance->initKeywordDetect(TEST_SAMPLE_RATE, TEST_CHANNELS);
	bool ret = instance->deinitKeywordDetect();
	TC_ASSERT_EQ("initKeywordDetect", ret, true);
	TC_SUCCESS_RESULT();
}

static void utc_media_SpeechDetector_deinitKeywordDetect_n(void)
{
	auto instance = media::voice::SpeechDetector::instance();
	bool ret = instance->deinitKeywordDetect();
	TC_ASSERT_EQ("initKeywordDetect", ret, false);
	TC_SUCCESS_RESULT();
}

static void utc_media_SpeechDetector_deinitEndPointDetect_p(void)
{
	auto instance = media::voice::SpeechDetector::instance();
	instance->initEndPointDetect(TEST_SAMPLE_RATE, TEST_CHANNELS);
	bool ret = instance->deinitEndPointDetect();
	TC_ASSERT_EQ("initEndPointDetect", ret, true);
	TC_SUCCESS_RESULT();
}

static void utc_media_SpeechDetector_deinitEndPointDetect_n(void)
{
	auto instance = media::voice::SpeechDetector::instance();
	bool ret = instance->deinitEndPointDetect();
	TC_ASSERT_EQ("initEndPointDetect", ret, false);
	TC_SUCCESS_RESULT();
}

static void utc_media_SpeechDetector_setEndPointDetectedDelegate_p(void)
{
	TC_SUCCESS_RESULT();
}

static void utc_media_SpeechDetector_startKeywordDetect_p(void)
{
	auto instance = media::voice::SpeechDetector::instance();
	instance->initKeywordDetect(TEST_SAMPLE_RATE, TEST_CHANNELS);
	bool ret = instance->startKeywordDetect(0);
	TC_ASSERT_EQ_CLEANUP("initKeywordDetect", ret, true, goto cleanup);
	TC_SUCCESS_RESULT();
cleanup:
	instance->deinitKeywordDetect();
}

static void utc_media_SpeechDetector_startKeywordDetect_n(void)
{
	auto instance = media::voice::SpeechDetector::instance();
	bool ret = instance->startKeywordDetect(0);
	TC_ASSERT_EQ_CLEANUP("initKeywordDetect", ret, false, goto cleanup);
	TC_SUCCESS_RESULT();
cleanup:
	instance->deinitKeywordDetect();
}

static void utc_media_SpeechDetector_startEndPointDetect_p(void)
{
	auto instance = media::voice::SpeechDetector::instance();
	instance->initEndPointDetect(TEST_SAMPLE_RATE, TEST_CHANNELS);
	bool ret = instance->startEndPointDetect(0);
	TC_ASSERT_EQ_CLEANUP("initEndPointDetect", ret, true, goto cleanup);
	TC_SUCCESS_RESULT();
cleanup:
	instance->deinitEndPointDetect();
}

static void utc_media_SpeechDetector_startEndPointDetect_n(void)
{
	auto instance = media::voice::SpeechDetector::instance();
	bool ret = instance->startEndPointDetect(0);
	TC_ASSERT_EQ_CLEANUP("initEndPointDetect", ret, false, goto cleanup);
	TC_SUCCESS_RESULT();
cleanup:
	instance->deinitEndPointDetect();
}

static void utc_media_SpeechDetector_processEPDFrame_p(void)
{
	TC_SUCCESS_RESULT();
}

int utc_media_SpeechDetector_main(void)
{
	utc_media_SpeechDetector_instance_p();
	utc_media_SpeechDetector_initKeywordDetect_p();
	utc_media_SpeechDetector_initKeywordDetect_n();
	utc_media_SpeechDetector_initEndPointDetect_p();
	utc_media_SpeechDetector_initEndPointDetect_n();
	utc_media_SpeechDetector_deinitKeywordDetect_p();
	utc_media_SpeechDetector_deinitKeywordDetect_n();
	utc_media_SpeechDetector_deinitEndPointDetect_p();
	utc_media_SpeechDetector_deinitEndPointDetect_n();
	utc_media_SpeechDetector_setEndPointDetectedDelegate_p();
	utc_media_SpeechDetector_startKeywordDetect_p();
	utc_media_SpeechDetector_startKeywordDetect_n();
	utc_media_SpeechDetector_startEndPointDetect_p();
	utc_media_SpeechDetector_startEndPointDetect_n();
	utc_media_SpeechDetector_processEPDFrame_p();
	return 0;
}
