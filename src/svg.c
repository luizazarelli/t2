#include "svg.h"

FILE *svg_criar(const char *caminho, double vx, double vy, double vw, double vh) {
    if (caminho == NULL)
        return NULL;
    FILE *f = fopen(caminho, "w+");
    if (f == NULL)
        return NULL;
    fprintf(f, "<?xml version='1.0' encoding='utf-8'?>\n");
    fprintf(f, "<svg:svg xmlns:svg=\"http://www.w3.org/2000/svg\" viewBox=\"%.1f %.1f %.1f %.1f\">\n",
            vx, vy, vw, vh);
    return f;
}

void svg_fechar(FILE *svg) {
    if (svg == NULL)
        return;
    fprintf(svg, "   </svg:g>\n</svg:svg>\n");
    fclose(svg);
}

void svg_retangulo(FILE *svg, double x, double y, double w, double h,
                   const char *fill, const char *stroke, double stroke_w) {
    if (svg == NULL)
        return;
    fprintf(svg,
            "   <svg:rect x=\"%.6f\" y=\"%.6f\" width=\"%.6f\" height=\"%.6f\" "
            "fill=\"%s\" stroke=\"%s\" stroke-width=\"%.6f\" />\n",
            x, y, w, h,
            fill   ? fill   : "none",
            stroke ? stroke : "none",
            stroke_w);
}

void svg_circulo(FILE *svg, double cx, double cy, double r,
                 const char *fill, const char *stroke, double stroke_w) {
    if (svg == NULL)
        return;
    fprintf(svg,
            "   <svg:circle cx=\"%.6f\" cy=\"%.6f\" r=\"%.6f\" "
            "stroke-opacity=\"0.5\" fill=\"%s\" stroke=\"%s\" stroke-width=\"%.6f\" />\n",
            cx, cy, r,
            fill   ? fill   : "none",
            stroke ? stroke : "none",
            stroke_w);
}

void svg_linha(FILE *svg, double x1, double y1, double x2, double y2,
               const char *stroke, double stroke_w) {
    if (svg == NULL)
        return;
    fprintf(svg,
            "   <svg:line x1=\"%.6f\" y1=\"%.6f\" x2=\"%.6f\" y2=\"%.6f\" "
            "stroke=\"%s\" stroke-width=\"%.6f\" />\n",
            x1, y1, x2, y2,
            stroke ? stroke : "black",
            stroke_w);
}

void svg_texto(FILE *svg, double x, double y,
               const char *texto, const char *fill, double font_size) {
    if (svg == NULL)
        return;
    fprintf(svg,
            "   <svg:text x=\"%.6f\" y=\"%.6f\" fill=\"%s\" font-size=\"%.0f\">%s</svg:text>\n",
            x, y,
            fill ? fill : "black",
            font_size,
            texto ? texto : "");
}

void svg_percursoAnimado(FILE *svg, const char *id,
                         double *xs, double *ys, int n,
                         const char *cor_rota, double stroke_w,
                         const char *cor_marcador, double dur) {
    if (svg == NULL || xs == NULL || ys == NULL || id == NULL || n < 2)
        return;

    fprintf(svg, "   <svg:path id=\"%s\" d=\"M %.6f,%.6f", id, xs[0], ys[0]);
    for (int i = 1; i < n; i++)
        fprintf(svg, " L %.6f,%.6f", xs[i], ys[i]);
    fprintf(svg, "\" stroke=\"%s\" stroke-width=\"%.6f\" fill=\"none\" />\n",
            cor_rota ? cor_rota : "black", stroke_w);

    fprintf(svg, "   <svg:g>\n");
    fprintf(svg, "      <svg:animateMotion dur=\"%.2fs\" repeatCount=\"indefinite\">\n", dur);
    fprintf(svg, "         <svg:mpath href=\"#%s\" />\n", id);
    fprintf(svg, "      </svg:animateMotion>\n");
    fprintf(svg, "      <svg:circle r=\"8\" fill=\"%s\" />\n",
            cor_marcador ? cor_marcador : "black");
    fprintf(svg, "      <svg:text text-anchor=\"middle\" dy=\"4\" font-size=\"10\" fill=\"white\">I</svg:text>\n");
    fprintf(svg, "   </svg:g>\n");

    fprintf(svg,
            "   <svg:circle cx=\"%.6f\" cy=\"%.6f\" r=\"8\" fill=\"%s\" />\n",
            xs[n - 1], ys[n - 1],
            cor_marcador ? cor_marcador : "black");
    fprintf(svg,
            "   <svg:text x=\"%.6f\" y=\"%.6f\" text-anchor=\"middle\" "
            "dy=\"4\" font-size=\"10\" fill=\"white\">F</svg:text>\n",
            xs[n - 1], ys[n - 1]);
}
