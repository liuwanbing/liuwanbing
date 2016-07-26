
#ifndef SCENE_FISH_TRACE_H_
#define SCENE_FISH_TRACE_H_
#pragma once

#include <vector>

typedef std::vector<FPointAngle> TraceVector;

extern TraceVector scene_kind_1_trace_[100 + 17 + 17 + 30 + 30 + 15 + 1];
extern TraceVector scene_kind_2_trace_[200 + 7 * 2];
extern TraceVector::size_type scene_kind_2_small_fish_stop_index_[200];
extern TraceVector::size_type scene_kind_2_small_fish_stop_count_;
extern TraceVector::size_type scene_kind_2_big_fish_stop_index_;
extern TraceVector::size_type scene_kind_2_big_fish_stop_count_;
extern TraceVector scene_kind_3_trace_[(50 + 40 + 30 + 1) * 2];
extern TraceVector scene_kind_4_trace_[8 * 8];
extern TraceVector scene_kind_5_trace_[(40 + 40 + 24 + 13 + 1) * 2];

// 场景7.jpg
void BuildSceneKind1Trace(float screen_width, float screen_height);
// 场景5.jpg
void BuildSceneKind2Trace(float screen_width, float screen_height);
// 场景1.jpg
void BuildSceneKind3Trace(float screen_width, float screen_height);
// 场景6.jpg
void BuildSceneKind4Trace(float screen_width, float screen_height);
// 场景3.jpg
void BuildSceneKind5Trace(float screen_width, float screen_height);

#endif  // SCENE_FISH_TRACE_H_