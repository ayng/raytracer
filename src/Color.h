#ifndef COLOR_H
#define COLOR_H

class Color {
  public:
    double r, g, b;
    Color ();
    Color (const double _r, const double _g, const double _b);
    Color (const Color& orig);
    Color operator + (const Color& rhs) const;
    Color operator * (const Color& rhs) const;
    Color operator * (const double rhs) const;
    Color clipped() const;
    void dump () const;
};

Color operator * (const double lhs, const Color& rhs);

#endif
