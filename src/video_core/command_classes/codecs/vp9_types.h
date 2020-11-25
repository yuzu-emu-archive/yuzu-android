// Copyright 2020 yuzu Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <array>
#include <cstring>
#include <vector>
#include "common/common_funcs.h"
#include "common/common_types.h"

namespace Tegra {
class GPU;

namespace Decoder {
struct Vp9FrameDimensions {
    s16 width{};
    s16 height{};
    s16 luma_pitch{};
    s16 chroma_pitch{};
};
static_assert(sizeof(Vp9FrameDimensions) == 0x8, "Vp9 Vp9FrameDimensions is an invalid size");

enum FrameFlags : u32 {
    IsKeyFrame = 1 << 0,
    LastFrameIsKeyFrame = 1 << 1,
    FrameSizeChanged = 1 << 2,
    ErrorResilientMode = 1 << 3,
    LastShowFrame = 1 << 4,
    IntraOnly = 1 << 5,
};

enum class TxSize {
    Tx4x4 = 0,   // 4x4 transform
    Tx8x8 = 1,   // 8x8 transform
    Tx16x16 = 2, // 16x16 transform
    Tx32x32 = 3, // 32x32 transform
    TxSizes = 4
};

enum class TxMode {
    Only4X4 = 0,      // Only 4x4 transform used
    Allow8X8 = 1,     // Allow block transform size up to 8x8
    Allow16X16 = 2,   // Allow block transform size up to 16x16
    Allow32X32 = 3,   // Allow block transform size up to 32x32
    TxModeSelect = 4, // Transform specified for each block
    TxModes = 5
};

struct Segmentation {
    u8 enabled{};
    u8 update_map{};
    u8 temporal_update{};
    u8 abs_delta{};
    std::array<u32, 8> feature_mask{};
    std::array<std::array<s16, 4>, 8> feature_data{};
};
static_assert(sizeof(Segmentation) == 0x64, "Segmentation is an invalid size");

struct LoopFilter {
    u8 mode_ref_delta_enabled{};
    std::array<s8, 4> ref_deltas{};
    std::array<s8, 2> mode_deltas{};
};
static_assert(sizeof(LoopFilter) == 0x7, "LoopFilter is an invalid size");

struct Vp9EntropyProbs {
    std::array<u8, 36> y_mode_prob{};
    std::array<u8, 64> partition_prob{};
    std::array<u8, 1728> coef_probs{};
    std::array<u8, 8> switchable_interp_prob{};
    std::array<u8, 28> inter_mode_prob{};
    std::array<u8, 4> intra_inter_prob{};
    std::array<u8, 5> comp_inter_prob{};
    std::array<u8, 10> single_ref_prob{};
    std::array<u8, 5> comp_ref_prob{};
    std::array<u8, 6> tx_32x32_prob{};
    std::array<u8, 4> tx_16x16_prob{};
    std::array<u8, 2> tx_8x8_prob{};
    std::array<u8, 3> skip_probs{};
    std::array<u8, 3> joints{};
    std::array<u8, 2> sign{};
    std::array<u8, 20> classes{};
    std::array<u8, 2> class_0{};
    std::array<u8, 20> prob_bits{};
    std::array<u8, 12> class_0_fr{};
    std::array<u8, 6> fr{};
    std::array<u8, 2> class_0_hp{};
    std::array<u8, 2> high_precision{};
};
static_assert(sizeof(Vp9EntropyProbs) == 0x7B4, "Vp9EntropyProbs is an invalid size");

struct Vp9PictureInfo {
    bool is_key_frame{};
    bool intra_only{};
    bool last_frame_was_key{};
    bool frame_size_changed{};
    bool error_resilient_mode{};
    bool last_frame_shown{};
    bool show_frame{};
    std::array<s8, 4> ref_frame_sign_bias{};
    s32 base_q_index{};
    s32 y_dc_delta_q{};
    s32 uv_dc_delta_q{};
    s32 uv_ac_delta_q{};
    bool lossless{};
    s32 transform_mode{};
    bool allow_high_precision_mv{};
    s32 interp_filter{};
    s32 reference_mode{};
    s8 comp_fixed_ref{};
    std::array<s8, 2> comp_var_ref{};
    s32 log2_tile_cols{};
    s32 log2_tile_rows{};
    bool segment_enabled{};
    bool segment_map_update{};
    bool segment_map_temporal_update{};
    s32 segment_abs_delta{};
    std::array<u32, 8> segment_feature_enable{};
    std::array<std::array<s16, 4>, 8> segment_feature_data{};
    bool mode_ref_delta_enabled{};
    bool use_prev_in_find_mv_refs{};
    std::array<s8, 4> ref_deltas{};
    std::array<s8, 2> mode_deltas{};
    Vp9EntropyProbs entropy{};
    Vp9FrameDimensions frame_size{};
    u8 first_level{};
    u8 sharpness_level{};
    u32 bitstream_size{};
    std::array<u64, 4> frame_offsets{};
    std::array<bool, 4> refresh_frame{};
};

struct Vp9FrameContainer {
    Vp9PictureInfo info{};
    std::vector<u8> bit_stream;
};

struct PictureInfo {
    INSERT_PADDING_WORDS(12);
    u32 bitstream_size{};
    INSERT_PADDING_WORDS(5);
    Vp9FrameDimensions last_frame_size{};
    Vp9FrameDimensions golden_frame_size{};
    Vp9FrameDimensions alt_frame_size{};
    Vp9FrameDimensions current_frame_size{};
    u32 vp9_flags{};
    std::array<s8, 4> ref_frame_sign_bias{};
    u8 first_level{};
    u8 sharpness_level{};
    u8 base_q_index{};
    u8 y_dc_delta_q{};
    u8 uv_ac_delta_q{};
    u8 uv_dc_delta_q{};
    u8 lossless{};
    u8 tx_mode{};
    u8 allow_high_precision_mv{};
    u8 interp_filter{};
    u8 reference_mode{};
    s8 comp_fixed_ref{};
    std::array<s8, 2> comp_var_ref{};
    u8 log2_tile_cols{};
    u8 log2_tile_rows{};
    Segmentation segmentation{};
    LoopFilter loop_filter{};
    INSERT_PADDING_BYTES(5);
    u32 surface_params{};
    INSERT_PADDING_WORDS(3);

    [[nodiscard]] Vp9PictureInfo Convert() const {
        return {
            .is_key_frame = (vp9_flags & FrameFlags::IsKeyFrame) != 0,
            .intra_only = (vp9_flags & FrameFlags::IntraOnly) != 0,
            .last_frame_was_key = (vp9_flags & FrameFlags::LastFrameIsKeyFrame) != 0,
            .frame_size_changed = (vp9_flags & FrameFlags::FrameSizeChanged) != 0,
            .error_resilient_mode = (vp9_flags & FrameFlags::ErrorResilientMode) != 0,
            .last_frame_shown = (vp9_flags & FrameFlags::LastShowFrame) != 0,
            .ref_frame_sign_bias = ref_frame_sign_bias,
            .base_q_index = base_q_index,
            .y_dc_delta_q = y_dc_delta_q,
            .uv_dc_delta_q = uv_dc_delta_q,
            .uv_ac_delta_q = uv_ac_delta_q,
            .lossless = lossless != 0,
            .transform_mode = tx_mode,
            .allow_high_precision_mv = allow_high_precision_mv != 0,
            .interp_filter = interp_filter,
            .reference_mode = reference_mode,
            .comp_fixed_ref = comp_fixed_ref,
            .comp_var_ref = comp_var_ref,
            .log2_tile_cols = log2_tile_cols,
            .log2_tile_rows = log2_tile_rows,
            .segment_enabled = segmentation.enabled != 0,
            .segment_map_update = segmentation.update_map != 0,
            .segment_map_temporal_update = segmentation.temporal_update != 0,
            .segment_abs_delta = segmentation.abs_delta,
            .segment_feature_enable = segmentation.feature_mask,
            .segment_feature_data = segmentation.feature_data,
            .mode_ref_delta_enabled = loop_filter.mode_ref_delta_enabled != 0,
            .use_prev_in_find_mv_refs = !(vp9_flags == (FrameFlags::ErrorResilientMode)) &&
                                        !(vp9_flags == (FrameFlags::FrameSizeChanged)) &&
                                        !(vp9_flags == (FrameFlags::IntraOnly)) &&
                                        (vp9_flags == (FrameFlags::LastShowFrame)) &&
                                        !(vp9_flags == (FrameFlags::LastFrameIsKeyFrame)),
            .ref_deltas = loop_filter.ref_deltas,
            .mode_deltas = loop_filter.mode_deltas,
            .frame_size = current_frame_size,
            .first_level = first_level,
            .sharpness_level = sharpness_level,
            .bitstream_size = bitstream_size,
        };
    }
};
static_assert(sizeof(PictureInfo) == 0x100, "PictureInfo is an invalid size");

struct EntropyProbs {
    INSERT_PADDING_BYTES(1024);
    std::array<u8, 28> inter_mode_prob{};
    std::array<u8, 4> intra_inter_prob{};
    INSERT_PADDING_BYTES(80);
    std::array<u8, 2> tx_8x8_prob{};
    std::array<u8, 4> tx_16x16_prob{};
    std::array<u8, 6> tx_32x32_prob{};
    std::array<u8, 4> y_mode_prob_e8{};
    std::array<std::array<u8, 8>, 4> y_mode_prob_e0e7{};
    INSERT_PADDING_BYTES(64);
    std::array<u8, 64> partition_prob{};
    INSERT_PADDING_BYTES(10);
    std::array<u8, 8> switchable_interp_prob{};
    std::array<u8, 5> comp_inter_prob{};
    std::array<u8, 3> skip_probs{};
    INSERT_PADDING_BYTES(1);
    std::array<u8, 3> joints{};
    std::array<u8, 2> sign{};
    std::array<u8, 2> class_0{};
    std::array<u8, 6> fr{};
    std::array<u8, 2> class_0_hp{};
    std::array<u8, 2> high_precision{};
    std::array<u8, 20> classes{};
    std::array<u8, 12> class_0_fr{};
    std::array<u8, 20> pred_bits{};
    std::array<u8, 10> single_ref_prob{};
    std::array<u8, 5> comp_ref_prob{};
    INSERT_PADDING_BYTES(17);
    std::array<u8, 2304> coef_probs{};

    void Convert(Vp9EntropyProbs& fc) {
        fc.inter_mode_prob = std::move(inter_mode_prob);
        fc.intra_inter_prob = std::move(intra_inter_prob);
        fc.tx_8x8_prob = std::move(tx_8x8_prob);
        fc.tx_16x16_prob = std::move(tx_16x16_prob);
        fc.tx_32x32_prob = std::move(tx_32x32_prob);

        for (s32 i = 0; i < 4; i++) {
            for (s32 j = 0; j < 9; j++) {
                fc.y_mode_prob[j + 9 * i] = j < 8 ? y_mode_prob_e0e7[i][j] : y_mode_prob_e8[i];
            }
        }

        fc.partition_prob = std::move(partition_prob);
        fc.switchable_interp_prob = std::move(switchable_interp_prob);
        fc.comp_inter_prob = std::move(comp_inter_prob);
        fc.skip_probs = std::move(skip_probs);
        fc.joints = std::move(joints);
        fc.sign = std::move(sign);
        fc.class_0 = std::move(class_0);
        fc.fr = std::move(fr);
        fc.class_0_hp = std::move(class_0_hp);
        fc.high_precision = std::move(high_precision);
        fc.classes = std::move(classes);
        fc.class_0_fr = std::move(class_0_fr);
        fc.prob_bits = std::move(pred_bits);
        fc.single_ref_prob = std::move(single_ref_prob);
        fc.comp_ref_prob = std::move(comp_ref_prob);

        // Skip the 4th element as it goes unused
        for (std::size_t i = 0; i < coef_probs.size(); i += 4) {
            const std::size_t j = i - i / 4;
            fc.coef_probs[j] = coef_probs[i];
            fc.coef_probs[j + 1] = coef_probs[i + 1];
            fc.coef_probs[j + 2] = coef_probs[i + 2];
        }
    }
};
static_assert(sizeof(EntropyProbs) == 0xEA0, "EntropyProbs is an invalid size");

enum class Ref { Last, Golden, AltRef };

struct RefPoolElement {
    s64 frame{};
    Ref ref{};
    bool refresh{};
};

struct FrameContexts {
    s64 from{};
    bool adapted{};
    Vp9EntropyProbs probs{};
};

}; // namespace Decoder
}; // namespace Tegra
