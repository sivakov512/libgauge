#include "tu/json.h"
#include "cJSON.h"
#include "tu/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *read_file(const char *path) {
    FILE *file = fopen(path, "rb");
    if (!file) {
        return NULL;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return NULL;
    }

    long size = ftell(file);
    if (size < 0) {
        fclose(file);
        return NULL;
    }

    if (fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        return NULL;
    }

    char *buf = malloc((size_t) size + 1);
    if (!buf) {
        fclose(file);
        return NULL;
    }

    fread(buf, 1, (size_t) size, file);
    buf[size] = '\0';
    fclose(file);
    return buf;
}

bool tu_json_read_frame(const char *path, uint8_t *buf, size_t buf_len,
                        gauge_frame_t *out) {
    char *text = read_file(path);
    if (!text) {
        return false;
    }

    cJSON *root = cJSON_Parse(text);
    free(text);
    if (!root) {
        return false;
    }

    cJSON *jwidth = cJSON_GetObjectItem(root, "width");
    cJSON *jheight = cJSON_GetObjectItem(root, "height");
    cJSON *jbuf = cJSON_GetObjectItem(root, "buf");

    if (!cJSON_IsNumber(jwidth) || !cJSON_IsNumber(jheight) ||
        !cJSON_IsArray(jbuf)) {
        cJSON_Delete(root);
        return false;
    }

    size_t width = (size_t) jwidth->valueint;
    size_t height = (size_t) jheight->valueint;
    size_t pixels = width * height;

    if (pixels > buf_len || (size_t) cJSON_GetArraySize(jbuf) != pixels) {
        cJSON_Delete(root);
        return false;
    }

    cJSON *item = jbuf->child;
    for (size_t i = 0; i < pixels && item; i++, item = item->next) {
        buf[i] = (uint8_t) item->valueint;
    }

    out->buf = buf;
    out->buf_len = pixels;
    out->width = width;
    out->height = height;

    cJSON_Delete(root);
    return true;
}

bool tu_json_write_frame(const char *path, const gauge_frame_t *frame) {
    cJSON *root = cJSON_CreateObject();
    if (!root) {
        return false;
    }

    cJSON_AddNumberToObject(root, "width", (double) frame->width);
    cJSON_AddNumberToObject(root, "height", (double) frame->height);

    cJSON *jbuf = cJSON_AddArrayToObject(root, "buf");
    for (size_t i = 0; i < frame->buf_len; i++) {
        cJSON_AddItemToArray(jbuf, cJSON_CreateNumber(frame->buf[i]));
    }

    char *text = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    if (!text) {
        return false;
    }

    tu_ensure_parent_dir(path);
    FILE *file = fopen(path, "wb");
    if (!file) {
        free(text);
        return false;
    }

    fputs(text, file);
    fclose(file);
    free(text);
    return true;
}

bool tu_json_read_line(const char *path, gauge_line_t *out) {
    char *text = read_file(path);
    if (!text) {
        return false;
    }

    cJSON *root = cJSON_Parse(text);
    free(text);
    if (!root) {
        return false;
    }

    cJSON *jox = cJSON_GetObjectItem(root, "origin_x");
    cJSON *joy = cJSON_GetObjectItem(root, "origin_y");
    cJSON *jdx = cJSON_GetObjectItem(root, "direction_x");
    cJSON *jdy = cJSON_GetObjectItem(root, "direction_y");

    if (!cJSON_IsNumber(jox) || !cJSON_IsNumber(joy) || !cJSON_IsNumber(jdx) ||
        !cJSON_IsNumber(jdy)) {
        cJSON_Delete(root);
        return false;
    }

    out->origin.x = (float) jox->valuedouble;
    out->origin.y = (float) joy->valuedouble;
    out->direction.x = (float) jdx->valuedouble;
    out->direction.y = (float) jdy->valuedouble;

    cJSON_Delete(root);
    return true;
}

bool tu_json_write_line(const char *path, const gauge_line_t *line) {
    cJSON *root = cJSON_CreateObject();
    if (!root) {
        return false;
    }

    cJSON_AddNumberToObject(root, "origin_x", (double) line->origin.x);
    cJSON_AddNumberToObject(root, "origin_y", (double) line->origin.y);
    cJSON_AddNumberToObject(root, "direction_x", (double) line->direction.x);
    cJSON_AddNumberToObject(root, "direction_y", (double) line->direction.y);

    char *text = cJSON_Print(root);
    cJSON_Delete(root);
    if (!text) {
        return false;
    }

    tu_ensure_parent_dir(path);
    FILE *file = fopen(path, "wb");
    if (!file) {
        free(text);
        return false;
    }

    fputs(text, file);
    fclose(file);
    free(text);
    return true;
}

bool tu_json_read_calibration(const char *path, gauge_calibration_data_t *out) {
    char *text = read_file(path);
    if (!text) {
        return false;
    }

    cJSON *root = cJSON_Parse(text);
    free(text);
    if (!root) {
        return false;
    }

    cJSON *jpx = cJSON_GetObjectItem(root, "pivot_x");
    cJSON *jpy = cJSON_GetObjectItem(root, "pivot_y");
    cJSON *jstart = cJSON_GetObjectItem(root, "angle_start_rad");
    cJSON *jend = cJSON_GetObjectItem(root, "angle_end_rad");
    cJSON *jspin = cJSON_GetObjectItem(root, "spin");
    cJSON *jlen = cJSON_GetObjectItem(root, "arrow_len");

    if (!cJSON_IsNumber(jpx) || !cJSON_IsNumber(jpy) || !cJSON_IsNumber(jstart) ||
        !cJSON_IsNumber(jend) || !cJSON_IsNumber(jspin) || !cJSON_IsNumber(jlen)) {
        cJSON_Delete(root);
        return false;
    }

    out->pivot.x = (size_t) jpx->valueint;
    out->pivot.y = (size_t) jpy->valueint;
    out->angle_start_rad = (float) jstart->valuedouble;
    out->angle_end_rad = (float) jend->valuedouble;
    out->spin = (gauge_spin_t) jspin->valueint;
    out->arrow_len = (size_t) jlen->valueint;

    cJSON_Delete(root);
    return true;
}

bool tu_json_write_calibration(const char *path,
                               const gauge_calibration_data_t *ca_data) {
    cJSON *root = cJSON_CreateObject();
    if (!root) {
        return false;
    }

    cJSON_AddNumberToObject(root, "pivot_x", (double) ca_data->pivot.x);
    cJSON_AddNumberToObject(root, "pivot_y", (double) ca_data->pivot.y);
    cJSON_AddNumberToObject(root, "angle_start_rad",
                            (double) ca_data->angle_start_rad);
    cJSON_AddNumberToObject(root, "angle_end_rad", (double) ca_data->angle_end_rad);
    cJSON_AddNumberToObject(root, "spin", (double) ca_data->spin);
    cJSON_AddNumberToObject(root, "arrow_len", (double) ca_data->arrow_len);

    char *text = cJSON_Print(root);
    cJSON_Delete(root);
    if (!text) {
        return false;
    }

    tu_ensure_parent_dir(path);
    FILE *file = fopen(path, "wb");
    if (!file) {
        free(text);
        return false;
    }

    fputs(text, file);
    fclose(file);
    free(text);
    return true;
}

bool tu_json_read_float(const char *path, float *out) {
    char *text = read_file(path);
    if (!text) {
        return false;
    }

    cJSON *root = cJSON_Parse(text);
    free(text);
    if (!root || !cJSON_IsNumber(root)) {
        cJSON_Delete(root);
        return false;
    }

    *out = (float) root->valuedouble;
    cJSON_Delete(root);
    return true;
}

bool tu_json_write_float(const char *path, float value) {
    cJSON *root = cJSON_CreateNumber((double) value);
    if (!root) {
        return false;
    }

    char *text = cJSON_Print(root);
    cJSON_Delete(root);
    if (!text) {
        return false;
    }

    tu_ensure_parent_dir(path);
    FILE *file = fopen(path, "wb");
    if (!file) {
        free(text);
        return false;
    }

    fputs(text, file);
    fclose(file);
    free(text);
    return true;
}
