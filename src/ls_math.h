#define PI 3.14159265358979323846
#define ONE_DEGREE_IN_RADIANS PI / 180.0f
#define ONE_RADIAN_IN_DEGREES 180.0f / PI

#define M4X4_IDENTITY {{ 1.0f,0.0f, 0.0f, 0.0f, 0.0f,1.0f, 0.0f, 0.0f, 0.0f,0.0f, 1.0f, 0.0f, 0.0f,0.0f, 0.0f, 1.0f }}

#define MIN_NEG_R32 (-FLT_MAX)
#define MAX_NEG_R32 (-FLT_MIN)
#define MIN_POS_R32 FLT_MIN
#define MAX_POS_R32 FLT_MAX

union v2 {
    struct {
        r32 x,y;
    };
    r32 E[2];
};

union v2i {
    struct {
        s32 x,y;
    };
    s32 E[2];
};

union v3{
    struct {
        r32 x,y,z;
    };
    struct {
        r32 r,g,b;
    };
    struct {
        r32 Hue,Sat,Val;
    };

    v2 xy;
    r32 E[3];
};

union v4{
    struct {
        r32 x,y,z,w;
    };
    struct {
        r32 r,g,b;
    };
    struct {
        r32 Hue,Sat,Val;
    };
    struct {
        v3 _ignore;
        r32 a;
    };
    v3 rgb;
    v3 xyz;
    v3 hsv;
    r32 E[4];
};

struct mat3x3 {
    r32 E[9];
};

union m4x4 {
    struct {
        v4 Rows[4];
    };
    r32 E[16];
};

union uv {
    struct {
        v2 TopLeft;
        v2 TopRight;
        v2 BottomLeft;
        v2 BottomRight;
    };
    v2 E[4];
};

struct rect {
    v2 Min;
    v2 Max;

    r32 Width() { return this->Max.x - this->Min.x; }
    r32 Height() { return this->Max.y - this->Min.y; }
    v2 Dim() { return {this->Max.x - this->Min.x, this->Max.y - this->Min.y}; }
};

struct rect4 {
    v2 BottomLeft;
    v2 TopLeft;
    v2 TopRight;
    v2 BottomRight;
};

inline b32
IsBetween(u32 Number, u32 Min, u32 Max) {
    return((Number > Min) && (Number < Max));
}

inline r32
SquareRoot(r32 A)
{
    r32 Result = sqrtf(A);
    return(Result);
}

inline r32
Square(r32 A)
{
    r32 Result = A * A;
    return(Result);
}

inline u32
SafeTruncateU64(u64 Value)
{
    Assert(Value <= 0xffffffff);
    u32 Result = (u32)Value;

    return(Result);
}

inline r32
SafeDivide0(r32 A, r32 B)
{
    r32 Result;

    if (B == 0) {
        Result = 0;
    } else {
        Result = A / B;
    }

    return(Result);
}

inline r32
SafeDivide1(r32 A, r32 B)
{
    r32 Result;

    if (B == 0) {
        Result = 1;
    } else {
        Result = A / B;
    }

    return(Result);
}

u32
Power(u32 Number, u32 P) {
    u32 Result = 1;

    while (P > 0) {
        Result *= Number;
        --P;
    }

    return (Result);
}

inline b32
NullV2(v2 A)
{
    return ((A.x == 0.0f) && (A.y == 0.0f));
}

v2
Perp(v2 A)
{
    v2 Result = {A.y * -1.0f, A.x};
    return Result;
}

inline b32
Equal(v2 A, v2 B)
{
    b32 Result = ((A.x == B.x) && (A.y == B.y));

    return(Result);
}

inline v2
operator*(v2 A, r32 B)
{
    v2 Result;

    Result.x = A.x * B;
    Result.y = A.y * B;

    return(Result);
}

inline v3
operator*(mat3x3 Matrix, v3 A)
{
    v3 Result = {};

    Result.x = Matrix.E[0] * A.x + Matrix.E[1] * A.y + Matrix.E[2] * A.z;
    Result.y = Matrix.E[3] * A.x + Matrix.E[4] * A.y + Matrix.E[5] * A.z;
    Result.z = Matrix.E[6] * A.x + Matrix.E[7] * A.y + Matrix.E[8] * A.z;

    return(Result);
}

inline v3
operator*(m4x4 Matrix, v3 A)
{
    v3 Result = {};

    v4 AHomogenous = {};
    AHomogenous.x = A.x;
    AHomogenous.y = A.y;
    AHomogenous.z = A.z;
    AHomogenous.w = 1.0f;

    Result.x = Matrix.E[0] * A.x + Matrix.E[1] * A.y + Matrix.E[2] * A.z + Matrix.E[3];
    Result.y = Matrix.E[4] * A.x + Matrix.E[5] * A.y + Matrix.E[6] * A.z + Matrix.E[7];
    Result.z = Matrix.E[8] * A.x + Matrix.E[9] * A.y + Matrix.E[10] * A.z + Matrix.E[11];

    return(Result);
}

inline m4x4
operator*(m4x4 A, m4x4 B)
{
    m4x4 Result = {};

    Result.Rows[0] = {
        A.Rows[0].x * B.Rows[0].x + A.Rows[0].y * B.Rows[1].x + A.Rows[0].z * B.Rows[2].x + A.Rows[0].w * B.Rows[3].x,
        A.Rows[0].x * B.Rows[0].y + A.Rows[0].y * B.Rows[1].y + A.Rows[0].z * B.Rows[2].y + A.Rows[0].w * B.Rows[3].y,
        A.Rows[0].x * B.Rows[0].z + A.Rows[0].y * B.Rows[1].z + A.Rows[0].z * B.Rows[2].z + A.Rows[0].w * B.Rows[3].z,
        A.Rows[0].x * B.Rows[0].w + A.Rows[0].y * B.Rows[1].w + A.Rows[0].z * B.Rows[2].w + A.Rows[0].w * B.Rows[3].w,
    };

    Result.Rows[1] = {
        A.Rows[1].x * B.Rows[0].x + A.Rows[1].y * B.Rows[1].x + A.Rows[1].z * B.Rows[2].x + A.Rows[1].w * B.Rows[3].x,
        A.Rows[1].x * B.Rows[0].y + A.Rows[1].y * B.Rows[1].y + A.Rows[1].z * B.Rows[2].y + A.Rows[1].w * B.Rows[3].y,
        A.Rows[1].x * B.Rows[0].z + A.Rows[1].y * B.Rows[1].z + A.Rows[1].z * B.Rows[2].z + A.Rows[1].w * B.Rows[3].z,
        A.Rows[1].x * B.Rows[0].w + A.Rows[1].y * B.Rows[1].w + A.Rows[1].z * B.Rows[2].w + A.Rows[1].w * B.Rows[3].w,
    };

    Result.Rows[2] = {
        A.Rows[2].x * B.Rows[0].x + A.Rows[2].y * B.Rows[1].x + A.Rows[2].z * B.Rows[2].x + A.Rows[2].w * B.Rows[3].x,
        A.Rows[2].x * B.Rows[0].y + A.Rows[2].y * B.Rows[1].y + A.Rows[2].z * B.Rows[2].y + A.Rows[2].w * B.Rows[3].y,
        A.Rows[2].x * B.Rows[0].z + A.Rows[2].y * B.Rows[1].z + A.Rows[2].z * B.Rows[2].z + A.Rows[2].w * B.Rows[3].z,
        A.Rows[2].x * B.Rows[0].w + A.Rows[2].y * B.Rows[1].w + A.Rows[2].z * B.Rows[2].w + A.Rows[2].w * B.Rows[3].w,
    };

    Result.Rows[3] = {
        A.Rows[3].x * B.Rows[0].x + A.Rows[3].y * B.Rows[1].x + A.Rows[3].z * B.Rows[2].x + A.Rows[3].w * B.Rows[3].x,
        A.Rows[3].x * B.Rows[0].y + A.Rows[3].y * B.Rows[1].y + A.Rows[3].z * B.Rows[2].y + A.Rows[3].w * B.Rows[3].y,
        A.Rows[3].x * B.Rows[0].z + A.Rows[3].y * B.Rows[1].z + A.Rows[3].z * B.Rows[2].z + A.Rows[3].w * B.Rows[3].z,
        A.Rows[3].x * B.Rows[0].w + A.Rows[3].y * B.Rows[1].w + A.Rows[3].z * B.Rows[2].w + A.Rows[3].w * B.Rows[3].w,
    };

    return(Result);
}

inline v2
operator/(v2 A, r32 B)
{
    v2 Result = A * (1.0f/B);
    return(Result);
}

inline v2
operator*(r32 B, v2 A)
{
    v2 Result;

    Result = A * B;

    return(Result);
}

inline v3
operator*(v3 A, r32 B)
{
    v3 Result;

    Result.x = A.x * B;
    Result.y = A.y * B;
    Result.z = A.z * B;

    return(Result);
}

inline v3
operator*(r32 B, v3 A)
{
    v3 Result;

    Result = A * B;

    return(Result);
}


inline v3
operator/(v3 A, r32 B)
{
    v3 Result = A * (1.0f/B);
    return(Result);
}

inline v2
operator+(v2 A, r32 B)
{
    v2 Result;

    Result.x = A.x + B;
    Result.y = A.y + B;

    return(Result);
}

inline v2
operator+(r32 B, v2 A)
{
    v2 Result;

    Result = A + B;

    return(Result);
}


inline v3
operator+(v3 A, r32 B)
{
    v3 Result;

    Result.x = A.x + B;
    Result.y = A.y + B;
    Result.z = A.z + B;

    return(Result);
}

inline v3
operator+(r32 B, v3 A)
{
    v3 Result;

    Result = A + B;

    return(Result);
}

inline v3
operator+(v3 A, v3 B)
{
    v3 Result;

    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    Result.z = A.z + B.z;

    return(Result);
}
inline v3
operator-(v3 A, v3 B)
{
    v3 Result;

    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    Result.z = A.z - B.z;

    return(Result);
}

inline v2
operator+(v2 A, v2 B)
{
    v2 Result;

    Result.x = A.x + B.x;
    Result.y = A.y + B.y;

    return(Result);
}

inline v4
operator+(v4 A, v4 B)
{
    v4 Result;

    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    Result.z = A.z + B.z;
    Result.w = A.w + B.w;

    return(Result);
}

inline v4
operator-(v4 A, v4 B)
{
    v4 Result;

    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    Result.z = A.z - B.z;
    Result.w = A.w - B.w;

    return(Result);
}

inline v4
operator*(v4 A, r32 B)
{
    v4 Result;

    Result.x = A.x * B;
    Result.y = A.y * B;
    Result.z = A.z * B;
    Result.w = A.w * B;

    return(Result);
}

inline v4
operator*(r32 B, v4 A)
{
    v4 Result;

    Result = A * B;

    return(Result);
}


inline v4
operator/(v4 A, r32 B)
{
    v4 Result = A * (1.0f / B);
    return(Result);
}


inline v2
operator-(v2 A, v2 B)
{
    v2 Result;

    Result.x = A.x - B.x;
    Result.y = A.y - B.y;

    return(Result);
}

inline v2 &
operator+=(v2 &A, v2 B)
{
    A = A + B;
    return(A);
}

inline v2 &
operator-=(v2 &A, v2 B)
{
    A = A - B;
    return(A);
}

inline v3 &
operator+=(v3 &A, v3 B)
{
    A = A + B;
    return(A);
}

inline v3 &
operator-=(v3 &A, v3 B)
{
    A = A - B;
    return(A);
}

inline v2
V2(r32 X, r32 Y) {
    v2 Result;
    Result.x = X;
    Result.y = Y;

    return(Result);
}

inline v3
V3(r32 X, r32 Y, r32 Z) {
    v3 Result;
    Result.x = X;
    Result.y = Y;
    Result.z = Z;

    return(Result);
}

inline v3
V3(v2 XY, r32 Z) {
    v3 Result;
    Result.x = XY.x;
    Result.y = XY.y;
    Result.z = Z;

    return(Result);
}

inline v4
V4(r32 X, r32 Y, r32 Z, r32 W) {
    v4 Result;
    Result.x = X;
    Result.y = Y;
    Result.z = Z;
    Result.w = W;

    return(Result);
}

inline v4
RGBA(u32 R, u32 G, u32 B, u32 A) {
    v4 Result;
    Result.r = R;
    Result.g = G;
    Result.b = B;
    Result.a = A;

    return(Result);
}

inline v4
RgbaToHsva(v4 Rgba)
{
    return Rgba;
}

inline v4
RGBALinear(u32 R, u32 G, u32 B, u32 A) {
    v4 Result;
    Result.r = pow(r32(R) / 255.0f, 1.0f / 2.2f) * 255.0f;
    Result.g = pow(r32(G) / 255.0f, 1.0f / 2.2f) * 255.0f;
    Result.b = pow(r32(B) / 255.0f, 1.0f / 2.2f) * 255.0f;
    Result.a = A;

    return(Result);
}

inline b32
Equal(v4 A, v4 B) {
    return ((A.x == B.x) && (A.y == B.y) && (A.z == B.z) && (A.w == B.w));
}

inline b32
Equal(rect A, rect B)
{
    b32 Result = Equal(A.Min, B.Min) && Equal(A.Max, B.Max);

    return(Result);
}

inline v2
RectCenter(rect Rect)
{
    return(Rect.Min + V2((Rect.Max.x - Rect.Min.x)/2.0f, (Rect.Max.y - Rect.Min.y) / 2.0f));
}

inline v2
RectDim(rect Rect)
{
    return(V2(Rect.Max.x - Rect.Min.x, Rect.Max.y - Rect.Min.y));
}

inline rect
RectCenterDim(v2 Center, r32 DimX, r32 DimY)
{
    rect Result;
    Result.Min = V2((Center.x - DimX/2), (Center.y - DimY/2));
    Result.Max = V2((Center.x + DimX/2), (Center.y + DimY/2));

    return(Result);
}

inline rect
RectPDim(v2 P, v2 Dim)
{
    rect Result;
    Result.Min = P;
    Result.Max = P + Dim;

    return(Result);
}

b32
RectanglesIntersect(rect A, rect B)
{
    b32 Result = false;

    if((((A.Min.y <= B.Max.y) && (A.Min.y >= B.Min.y)) ||
        ((B.Min.y <= A.Max.y) && (B.Min.y >= A.Min.y))) &&
        (((A.Min.x <= B.Max.x) || (A.Min.x >= B.Min.x)) && ((B.Min.x <= A.Max.x) || (B.Min.x >= A.Min.x))))
    {
        Result = true;
    }

    return(Result);
}

inline rect
RectTopLeftWidthHeight(r32 Left, r32 Top, r32 Width, r32 Height)
{
    rect Result;
    Result.Min = V2(Left, Top);
    Result.Max = V2(Left + Width, Top + Height);

    return(Result);
}

inline b32
InRect(rect Rect, r32 X, r32 Y)
{
    b32 Result = false;

    if((X >= Rect.Min.x) && (X <= Rect.Max.x) && ((Y >= Rect.Min.y) && (Y <= Rect.Max.y))) {
        Result = true;
    }

    return(Result);
}

inline b32
InRect(rect Rect, v2 P)
{
    b32 Result = false;

    if((P.x >= Rect.Min.x) && (P.x <= Rect.Max.x) && ((P.y >= Rect.Min.y) && (P.y <= Rect.Max.y))) {
        Result = true;
    }

    return(Result);
}

inline r32
Inner(v3 A, v3 B)
{
    r32 Result = A.x*B.x + A.y*B.y + A.z*B.z;

    return(Result);
}

inline r32
Inner(v2 A, v2 B)
{
    r32 Result = A.x*B.x + A.y*B.y;

    return(Result);
}

inline r32
Dot(v2 A, v2 B)
{
    r32 Result = A.x*B.x + A.y*B.y;

    return(Result);
}

inline v3
Cross(v3 A, v3 B)
{
    v3 X = V3(1.0f, 0.0f, 0.0f);
    v3 Y = V3(0.0f, 1.0f, 0.0f);
    v3 Z = V3(1.0f, 0.0f, 1.0f);

    v3 Result = X*(A.y*B.z - A.z*B.y) - Y*(A.x*B.z - A.z*B.x) + Z*(A.x*B.y - A.y*B.x);

    return(Result);
}

inline r32
LengthSq(v2 A)
{
    r32 Result = Inner(A, A);
    return(Result);
}

inline r32
LengthSq(v3 A)
{
    r32 Result = Inner(A, A);
    return(Result);
}

inline r32
Length(v2 A)
{
    r32 Result = SquareRoot(LengthSq(A));
    return(Result);
}

inline r32
Length(v3 A)
{
    r32 Result = SquareRoot(LengthSq(A));
    return(Result);
}

inline b32
InCircle(v2 Center, r32 R, r32 X, r32 Y)
{
    v2 P = {X, Y};
    return (Length(P - Center) <= R);
}

inline v2
Normalize(v2 A)
{
    if (A.x == 0.0f &&  A.y == 0.0f) {
        return {0.0, 0.0f};
    }

    v2 Result = A / Length(A);

    return (Result);
}

inline v2
NormalizeZero(v2 A)
{
    r32 Len = Length(A);

    if (Len == 0.0f) {
        return v2 {0.0f, 0.0f};
    }

    return A / Len;
}

inline v3
Normalize(v3 A)
{
    v3 Result = A / Length(A);
    return (Result);
}

r32
Lerp(r32 A, r32 B, r32 t) {
    assert((t >= 0.0f) && (t<= 1.0f));
    r32 Result;

    Result = (A * (1.0f - t)) + (B * t);

    return(Result);
}

v2
Lerp(v2 A, v2 B, r32 t) {
    assert((t >= 0.0f) && (t<= 1.0f));
    v2 Result;

    Result = (A * (1.0f - t)) + (B * t);

    return(Result);
}

v4
Lerp(v4 A, v4 B, r32 t) {
    assert((t >= 0.0f) && (t<= 1.0f));
    v4 Result;

    Result = (A * (1.0f - t)) + (B * t);

    return(Result);
}

inline r32
Sign(r32 A)
{
    r32 Result = 0.0f;
    if (A < 0.0f) {
        Result = -1.0f;
    } else {
        Result = 1.0f;
    }

    return(Result);
}

inline r32
Sign_r32(r32 A)
{
    r32 Result = 0.0f;
    if (A < 0.0f) {
        Result = -1.0f;
    } else {
        Result = 1.0f;
    }

    return(Result);
}

inline s32
Sign_s32(r32 A) { return A < 0 ? -1 : 1; }

inline r32
Abs(r32 A) {
    r32 Result;

    if (A < 0.0f) {
        Result =  A * -1.0f;
    }else {
        Result = A;
    }

    return(Result);
}

inline s32 Max_s32(s32 A, s32 B) { return B > A ? B : A; }
inline s32 Min_s32(s32 A, s32 B) { return B < A ? B : A; }
inline u32 Max_u32(u32 A, u32 B) { return B > A ? B : A; }
inline u32 Min_u32(u32 A, u32 B) { return B < A ? B : A; }
inline u32 Max_s16(s16 A, s16 B) { return B > A ? B : A; }
inline u32 Min_s16(s16 A, s16 B) { return B < A ? B : A; }
inline r32 Max_r32(r32 A, r32 B) { return B > A ? B : A; }
inline r32 Min_r32(r32 A, r32 B) { return B < A ? B : A; }

inline r32 Clamp_r32(r32 A, r32 Val, r32 B) { return Min_r32(Max_r32(A, Val), B); }
inline s32 Clamp_s32(s32 A, s32 Val, s32 B) { return Min_s32(Max_s32(A, Val), B); }
inline u32 Clamp_u32(u32 A, u32 Val, u32 B) { return Min_u32(Max_u32(A, Val), B); }

inline r64 Abs_r64(r64 A) { return A < 0 ? -A : A; }
inline r32 Abs_r32(r32 A) { return A < 0 ? -A : A; }
inline u8 Abs_u8(u8 A) { return A < 0 ? -A : A; }
inline u16 Abs_u16(u16 A) { return A < 0 ? -A : A; }
inline u32 Abs_u32(u32 A) { return A < 0 ? -A : A; }
inline u64 Abs_u64(u64 A) { return A < 0 ? -A : A; }
inline s8 Abs_s8(s8 A) { return A < 0 ? -A : A; }
inline s16 Abs_s16(s16 A) { return A < 0 ? -A : A; }
inline s32 Abs_s32(s32 A) { return A < 0 ? -A : A; }
inline s64 Abs_s64(s64 A) { return A < 0 ? -A : A; }

inline r32 Fraction_r32(r32 A) { return A - (r32)((u32)A); }

r32
Ceil_r32(r32 A)
{
    r32 Fraction = A - (r32)((u32)A);
    if (Fraction == 0.0f) {
        return A;
    }
    return (r32)((u32)(A + 1.0f));
}

inline r32 Floor_r32(r32 A) { return (r32)((u32)A); }

inline r32
Max(r32 A, r32 B)
{
    r32 Result;
    Result = A;
    if (B > A) {
        Result = B;
    }

    return(Result);
}

inline r32
Min(r32 A, r32 B)
{
    r32 Result;
    Result = A;
    if (B < A) {
        Result = B;
    }

    return(Result);
}
inline u32
Max(u32 A, u32 B)
{
    u32 Result;
    Result = A;
    if (B > A) {
        Result = B;
    }

    return(Result);
}

inline u64
Max(u64 A, u64 B)
{
    u64 Result;
    Result = A;
    if (B > A) {
        Result = B;
    }

    return(Result);
}

inline s32
Max(s32 A, s32 B)
{
    s32 Result;
    Result = A;
    if (B > A) {
        Result = B;
    }

    return(Result);
}

inline u32
Min(u32 A, u32 B)
{
    u32 Result;
    Result = A;
    if (B < A) {
        Result = B;
    }

    return(Result);
}
inline r32
Round(r32 A)
{
    return((r32)((u32)(A + 0.5f)));
}
v2
RotatePoint(v2 P, v2 Origin, r32 Degrees)
{
    v2 Result = P;

    if (Degrees != 0.0f) {
        v2 BaseX = {1.0f, 0.0f};
        v2 BaseY = {0.0f, 1.0f};

        r32 OneDegreeInRadians = PI / 180.0f;

        v2 LocalP = P - Origin;

        BaseX = V2(cos(Degrees * OneDegreeInRadians), sin(Degrees * OneDegreeInRadians));
        BaseY = V2(sin(Degrees * OneDegreeInRadians) * -1.0f, cos(Degrees * OneDegreeInRadians));

        Result = Origin + LocalP.x * BaseX + LocalP.y * BaseY;
    }

    return(Result);
}

void
RotateVertices(r32 Rotation, v2 Origin, v2 *Vertices, u32 VertexCount)
{
    if (Rotation != 0.0f) {
        for (u32 VertexIndex = 0; VertexIndex < VertexCount; ++VertexIndex) {
            if (Origin.x != Vertices[VertexIndex].x || Origin.y != Vertices[VertexIndex].y) {
                Vertices[VertexIndex] = RotatePoint(Vertices[VertexIndex], Origin,  Rotation);
            }
        }
    }
}

void
TranslateVertices(v2 P, v2 *Vertices, u32 VertexCount)
{
    for (u32 VertexIndex = 0; VertexIndex < VertexCount; ++VertexIndex) {
        Vertices[VertexIndex] = Vertices[VertexIndex] + P;
    }
}


inline v2
GetRectCenter(rect Rect)
{
    v2 Result = V2(Rect.Min.x + (Rect.Max.x - Rect.Min.x) / 2.0f, Rect.Min.y + (Rect.Max.y - Rect.Min.y) / 2.0f);
    return(Result);
}

rect
GetBoundingBox(v2 *Vertices, u32 VertexCount)
{
    rect Result = {};

    Result.Min = V2(99999, 99999);
    Result.Max = V2(-99999, -99999);

    for (u32 j = 0; j< VertexCount; ++j) {
        if (Vertices[j].x > Result.Max.x) {
            Result.Max.x = Vertices[j].x;
        }
        if (Vertices[j].y > Result.Max.y) {
            Result.Max.y = Vertices[j].y;
        }
        if (Vertices[j].x < Result.Min.x) {
            Result.Min.x = Vertices[j].x;
        }
        if (Vertices[j].y < Result.Min.y) {
            Result.Min.y = Vertices[j].y;
        }
    }

    return(Result);
}

b32
DoSimplex(v2 *Vertices, u32 *VertexCount, v2 *Direction)
{
    // note(Stas): Winding is super important.
    // Here Perp to AC must be pointing outside triangle
    // and to AB - inside.
    b32 Result = false;

    assert((*VertexCount > 1) && (*VertexCount <= 4));

    if (*VertexCount == 4) {
        assert(!"3D collision is not implemented.");
    }

    if (*VertexCount == 2) {
        // Note(stas):
        // [1] - A
        // [0] - B
        v2 AB = Vertices[0] - Vertices[1];
        v2 AO = Vertices[1] * -1.0f;

        if (Inner(AB, AO) > 0.0f) {
            // NOTE(Stas): Return [A,B]
        r32 DirectionTest = Inner(AO, Perp(AB));
           *Direction = Perp(AB) * DirectionTest;
       if (DirectionTest < 0.0f) {
           // NOTE(stas): Swap vertices to have consisent winding in triangle case
           v2 Temp = Vertices[0];
           Vertices[0] = Vertices[1];
           Vertices[1] = Temp;
       }
        } else {
            // NOTE(Stas): Return [A]
            Vertices[0] = Vertices[1];
            *VertexCount = 1;
        }
    } else if (*VertexCount == 3) {
        // Note(stas):
        // [2] - A
        // [1] - B
        // [0] - C
        v2 AC = Vertices[0] - Vertices[2];
        v2 AB = Vertices[1] - Vertices[2];
        v2 AO = Vertices[2] * -1.0f;

        if (Inner(Perp(AC), AO) > 0.0f) {
            if (Inner(AC, AO) > 0.0f) {
                // NOTE(Stas): Return [A,C]
                *VertexCount = 2;
                Vertices[1] = Vertices[2];
                *Direction = Perp(AC);
            } else {
            Vertices[0] = Vertices[2];
            *VertexCount = 1;
            *Direction = AO;
            }
        } else {
            if (Inner(-1.0f * Perp(AB), AO) > 0.0f) {
            if (Inner(AB, AO) > 0.0f) {
                Vertices[0] = Vertices[2];
            *VertexCount = 2;
            *Direction = -1.0f * Perp(AB);
            } else {
                Vertices[0] = Vertices[2];
            *VertexCount = 1;
            *Direction = AO;
            }
            } else {
                // NOTE(Stas): Origin is inside the triangle.
            Result = true;
        }
        }
    }

    return(Result);
}

void
FillCircleArchPoints(v2 *Points, r32 StartAngle, r32 EndAngle, u32 ArchPointCount, r32 R)
{
    u32 SegmentCount = (ArchPointCount + 1);
    r32 SegmentAngle = (EndAngle - StartAngle) / SegmentCount;
    v2 CurrentVector = V2(1.0f, 0.0f);

    if (StartAngle == 90) {
        CurrentVector = V2(0.0f, 1.0f);
    } else if (StartAngle == 180) {
        CurrentVector = V2(-1.0f, 0.0f);
    }  else if (StartAngle == 270) {
        CurrentVector = V2(0.0f, -1.0f);
    }  else if (StartAngle == 360) {
        CurrentVector = V2(1.0f, 0.0f);
    }

    for(u32 i = 0; i <= SegmentCount; ++i) {
        Points[i] = CurrentVector * R;
        CurrentVector = RotatePoint(CurrentVector, V2(0.0f, 0.0f), SegmentAngle);
    }
}

void
FillCirclePoints(v2 *Points, u32 SegmentCount, r32 R)
{
    r32 SegmentAngle = 360.0f / SegmentCount * -1.0f;
    v2 CurrentVector = V2(0.0f, 1.0f);

    for(u32 i = 0; i < SegmentCount; ++i) {
        Points[i] = CurrentVector * R;
        CurrentVector = RotatePoint(CurrentVector, V2(0.0f, 0.0f), SegmentAngle);
    }

    Points[SegmentCount] = Points[0];
}

u32
LoadRoundedRectVertices(v2 P, v2 Dim, r32 R, v2 *Vertices)
{
    u32 Result = 0;

    v2 Points[70];
    u32 ArchPointCount = 10;
    u32 TotalPointCount =  8 + (ArchPointCount * 4);

    u32 ObjectVertexOffset = 0;
    u32 CircleVertexOffset = 0;

    v2 RegularPoints[4];
    RegularPoints[0] = V2(0.0f, Dim.y);
    RegularPoints[1] = V2(0.0f, 0.0f);
    RegularPoints[2] = V2(Dim.x, 0.0f);
    RegularPoints[3] = V2(Dim.x, Dim.y);

    v2 OriginPoints[4];
    OriginPoints[0] = V2(R, Dim.y - R);
    OriginPoints[1] = V2(R, R);
    OriginPoints[2] = V2(Dim.x - R, R);
    OriginPoints[3] = V2(Dim.x - R, Dim.y - R);

    r32 DegreeOffset = 90.0f;
    r32 ArchDegrees = 90.0f;

    for (u32 i =0; i<4; ++i) {
        r32 Mod = (u32)DegreeOffset % 360;
        FillCircleArchPoints(Points + CircleVertexOffset, Mod, Mod + ArchDegrees, ArchPointCount, R);

        for (u32 j = 0; j < ArchPointCount + 2; ++j) {
            v2 Point = Points[j + CircleVertexOffset];
            *(Vertices + ObjectVertexOffset + j) = OriginPoints[i] + Point + P;
            ++Result;
        }

        CircleVertexOffset += 2 + ArchPointCount;
        ObjectVertexOffset += 2 + ArchPointCount;

        DegreeOffset += ArchDegrees;
    }

    return(Result);
}

u32
LoadRoundedRectVerticesMultipleR(v2 P, v2 Dim, r32 *Radii, v2 *Vertices)
{
    u32 Result = 0;

    v2 Points[70];
    u32 ArchPointCount = 10;
    u32 TotalPointCount =  8 + (ArchPointCount * 4);

    u32 ObjectVertexOffset = 0;
    u32 CircleVertexOffset = 0;

    v2 RegularPoints[4];
    RegularPoints[0] = V2(0.0f, Dim.y);
    RegularPoints[1] = V2(0.0f, 0.0f);
    RegularPoints[2] = V2(Dim.x, 0.0f);
    RegularPoints[3] = V2(Dim.x, Dim.y);

    v2 OriginPoints[4];
    OriginPoints[0] = V2(Radii[0], Dim.y - Radii[0]);
    OriginPoints[1] = V2(Radii[1], Radii[1]);
    OriginPoints[2] = V2(Dim.x - Radii[2], Radii[2]);
    OriginPoints[3] = V2(Dim.x - Radii[3], Dim.y - Radii[3]);

    r32 DegreeOffset = 90.0f;
    r32 ArchDegrees = 90.0f;

    for (u32 i =0; i<4; ++i) {
        if (Radii[i] == 0.0f) {
            *(Vertices + ObjectVertexOffset) = RegularPoints[i] + P;
            ++ObjectVertexOffset;
            ++Result;
        } else {
            r32 Mod = (u32)DegreeOffset % 360;
            FillCircleArchPoints(Points + CircleVertexOffset, Mod, Mod + ArchDegrees, ArchPointCount, Radii[i]);

            for (u32 j = 0; j < ArchPointCount + 2; ++j) {
                v2 Point = Points[j + CircleVertexOffset];
                *(Vertices + ObjectVertexOffset + j) = OriginPoints[i] + Point + P;
                ++Result;
            }

            CircleVertexOffset += 2 + ArchPointCount;
            ObjectVertexOffset += 2 + ArchPointCount;
        }

        DegreeOffset += ArchDegrees;
    }

    return(Result);
}

v2
SingleShapeSupport(u32 VertexCount, v3 *Vertices, v2 Direction)
{
    v2 Result = {};
    r32 MaxDistance = 0.0f;
    b32 PointFound = false;

    for (u32 VertexIndex = 0; VertexIndex < VertexCount; ++VertexIndex) {
        r32 DistanceSq = Inner(Vertices[VertexIndex].xy, Direction);
        if (((DistanceSq < MaxDistance) && (!PointFound)) ||
            (DistanceSq >= MaxDistance))
        {
            MaxDistance = DistanceSq;
            Result = Vertices[VertexIndex].xy;
            PointFound = true;
        }
    }

    return(Result);
}

v2
MinkowskiSumSupport(u32 ShapeAPointCount, v3 *ShapeAVertices, u32 ShapeBPointCount, v3 *ShapeBVertices, v2 Direction)
{
    v2 Result;
    v2 ShapeASupport = SingleShapeSupport(ShapeAPointCount, ShapeAVertices, Direction);
    v2 ShapeBSupport = SingleShapeSupport(ShapeBPointCount, ShapeBVertices, Direction * (-1.0f));

    Result = ShapeASupport - ShapeBSupport;

    return(Result);
}

b32
GJKIntersect(u32 ShapeAPointCount, v3 *ShapeAVertices, u32 ShapeBPointCount, v3 *ShapeBVertices)
{
    b32 Result;

    u32 IterationCount = 0;
    v2 TestVertices[4];
    u32 TestVertexCount = 0;

    v2 InitialVertex = ShapeAVertices[0].xy - ShapeBVertices[0].xy;
    TestVertices[TestVertexCount++] = InitialVertex;

    v2 SearchDirection = InitialVertex * -1.0f;

    if (Equal(InitialVertex, V2(0.0f, 0.0f))) {
        Result = true;
    } else {
        for(;;) {
        if (IterationCount > 10) {
                Result = false;
        break;
        }

            v2 NewSupportVertex = MinkowskiSumSupport(ShapeAPointCount, ShapeAVertices, ShapeBPointCount, ShapeBVertices, SearchDirection);
            if (Equal(NewSupportVertex, V2(0.0f, 0.0f))) {
                Result = true;
                break;
            }

            if (Inner(NewSupportVertex, SearchDirection) < 0.0f) {
                Result = false;
                break;
            }

            assert(TestVertexCount <= 3);
            TestVertices[TestVertexCount++] = NewSupportVertex;

            if (DoSimplex(TestVertices, &TestVertexCount, &SearchDirection)) {
                Result = true;
                break;
            }
        ++IterationCount;
        }
    }

    return(Result);
}

r32
SignedArea(v2 A, v2 B, v2 C)
{
    return ((A.x - C.x) * (B.y - C.y) - (A.y - C.y) * (B.x - C.x));
}

b32
IntersectSegments(v2 A0, v2 A1, v2 B0, v2 B1, v2 *P)
{
    b32 Result = false;

    r32 s1 = SignedArea(A0, A1, B0);
    r32 s2 = SignedArea(A0, A1, B1);

    if (s1 * s2 < 0.0f) {
        r32 s3 = SignedArea(B0, B1, A0);
        r32 s4 = SignedArea(B0, B1, A1);

        if (s3 * s4 < 0.0f) {
            r32 t = s3 / (s3 - s4);
            *P = A0 + t * (A1 - A0);
            Result = true;
        }
    } else if (s1 == 0.0f) {
        *P = B0;
        Result = true;
    } else if (s2 == 0.0f) {
        *P = B1;
        Result = true;
    }

    return(Result);
}

b32
IntersectExtendedSegments(v2 A0, v2 A1, v2 B0, v2 B1, v2 *P)
{
    v2 N = Normalize(Perp(B1 - B0));
    r32 Denominator = Dot(N, A1 - A0);

    if (Denominator == 0) {
        return false;
    } else {
        r32 t = Dot(N, B0 - A0) / Denominator;
        v2 Intersection = A0 + t *(A1 - A0);
        *P = Intersection;
    }

    return true;
}

inline r32
Clamp(r32 A, r32 Value, r32 B)
{
    r32 Result = Value;

    if (Value < A) {
        Result = A;
    }
    if (Value > B) {
        Result = B;
    }

    return(Result);
}

inline r32
ClampRight(r32 Value, r32 Max)
{
    r32 Result = Value;

    if (Result > Max) {
        Result = Max;
    }

   return Result;
}

inline r32
ClampLeft(r32 Min, r32 Value)
{
    r32 Result = Value;

    if (Result < Min) {
        Result = Min;
    }

   return Result;
}

inline r32
Mod_r32(r32 A, r32 B)
{
    r32 Div = floor(A / B);
    r32 Result = A - (Div * B);
    return Result;
}

inline v3
HsvToRgb(v3 Color)
{
    Color.Hue = Clamp_r32(0.0f, Color.Hue, 360.0f);

    r32 C = Color.Val * Color.Sat;
    r32 X = C * (1.0f - Abs_r32(Mod_r32(Color.Hue / 60.0f, 2.0f) - 1.0f));
    r32 M = Color.Val - C;

    v3 Result;

    if (Color.Hue >= 300) {
        Result = V3(C, 0.0f, X);
    } else if (Color.Hue >= 240) {
        Result = V3(X, 0.0f, C);
    } else if (Color.Hue >= 180) {
        Result = V3(0.0f, X, C);
    } else if (Color.Hue >= 120) {
        Result = V3(0.0f, C, X);
    } else if (Color.Hue >= 60) {
        Result = V3(X, C, 0.0f);
    } else {
        Result = V3(C, X, 0.0f);
    }

    Result = Result + M;

    return Result;
}

b32
EqualsZero_v2(v2 A) {
    return (A.x == 0 && A.y == 0.0);
}

b32
WithinThreshold_v2(v2 A, r32 T) {
    return (Abs_r32(A.x) <= T && Abs(A.y) <= T);
}

