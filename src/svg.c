#include "svg.h"

FILE *svg_criar(const char *caminho, double largura, double altura) {
    if (caminho == NULL)
        return NULL;
    FILE *f = fopen(caminho, "w");
    if (f == NULL)
        return NULL;
    fprintf(f, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(f, "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"%.2f\" height=\"%.2f\">\n",
            largura, altura);
    return f;
}

void svg_fechar(FILE *svg) {
    if (svg == NULL)
        return;
    fprintf(svg, "</svg>\n");
    fclose(svg);
}

void svg_retangulo(FILE *svg, double x, double y, double w, double h,
                   const char *fill, const char *stroke, double stroke_w) {
    if (svg == NULL)
        return;
    fprintf(svg,
            "<rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\" "
            "fill=\"%s\" stroke=\"%s\" stroke-width=\"%.2f\"/>\n",
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
            "<circle cx=\"%.2f\" cy=\"%.2f\" r=\"%.2f\" "
            "fill=\"%s\" stroke=\"%s\" stroke-width=\"%.2f\"/>\n",
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
            "<line x1=\"%.2f\" y1=\"%.2f\" x2=\"%.2f\" y2=\"%.2f\" "
            "stroke=\"%s\" stroke-width=\"%.2f\"/>\n",
            x1, y1, x2, y2,
            stroke ? stroke : "none",
            stroke_w);
}

void svg_texto(FILE *svg, double x, double y,
               const char *texto, const char *fill, double font_size) {
    if (svg == NULL || texto == NULL)
        return;
    fprintf(svg,
            "<text x=\"%.2f\" y=\"%.2f\" fill=\"%s\" font-size=\"%.2f\">%s</text>\n",
            x, y,
            fill ? fill : "black",
            font_size,
            texto);
}

void svg_percursoAnimado(FILE *svg, const char *id,
                         double *xs, double *ys, int n,
                         const char *cor_rota, double stroke_w,
                         const char *cor_marcador, double dur) {
    if (svg == NULL || xs == NULL || ys == NULL || id == NULL || n < 2)
        return;

    fprintf(svg, "<path id=\"%s\" d=\"M %.2f,%.2f", id, xs[0], ys[0]);
    for (int i = 1; i < n; i++)
        fprintf(svg, " L %.2f,%.2f", xs[i], ys[i]);
    fprintf(svg, "\" stroke=\"%s\" stroke-width=\"%.2f\" fill=\"none\"/>\n",
            cor_rota ? cor_rota : "black", stroke_w);

    fprintf(svg, "<g>\n");
    fprintf(svg, "  <animateMotion dur=\"%.2fs\" repeatCount=\"indefinite\">\n", dur);
    fprintf(svg, "    <mpath href=\"#%s\"/>\n", id);
    fprintf(svg, "  </animateMotion>\n");
    fprintf(svg, "  <circle r=\"8\" fill=\"%s\"/>\n",
            cor_marcador ? cor_marcador : "black");
    fprintf(svg, "  <text text-anchor=\"middle\" dy=\"4\" font-size=\"10\" fill=\"white\">I</text>\n");
    fprintf(svg, "</g>\n");

    fprintf(svg,
            "<circle cx=\"%.2f\" cy=\"%.2f\" r=\"8\" fill=\"%s\"/>\n",
            xs[n - 1], ys[n - 1],
            cor_marcador ? cor_marcador : "black");
    fprintf(svg,
            "<text x=\"%.2f\" y=\"%.2f\" text-anchor=\"middle\" "
            "dy=\"4\" font-size=\"10\" fill=\"white\">F</text>\n",
            xs[n - 1], ys[n - 1]);
}
