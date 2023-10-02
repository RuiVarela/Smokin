#include <UnitTest.hpp>

#include <vision/Mat.hpp>
#include <vision/Features.hpp>
#include <vision/Optimization.hpp>
#include <vision/Image.hpp>
#include <vision/Opticalflow.hpp>
#include <vision/Drawing.hpp>
#include <vision/Filter.hpp>

#include <math/Mathematics.hpp>
#include <File.hpp>

#include <iostream>
#include <cassert>
#include <cassert>


//
// Baseline Matrix implementation
//
typedef struct matrix{
    int rows, cols;
    double **data;
} matrix;

typedef struct LUP{
    matrix *L;
    matrix *U;
    int *P;
    int n;
} LUP;

static matrix make_matrix(int rows, int cols) {
    matrix m;
    m.rows = rows;
    m.cols = cols;
    m.data = static_cast<double**>(calloc(size_t(m.rows), sizeof(double *)));
    int i;
    for(i = 0; i < m.rows; ++i) m.data[i] = static_cast<double*>(calloc(size_t(m.cols), sizeof(double)));
    return m;
}

static matrix make_identity_homography() {
    matrix H = make_matrix(3,3);
    H.data[0][0] = 1;
    H.data[1][1] = 1;
    H.data[2][2] = 1;
    return H;
}

static matrix make_translation_homography(float dx, float dy) {
    matrix H = make_identity_homography();
    H.data[0][2] = double(dx);
    H.data[1][2] = double(dy);
    return H;
}

static void free_matrix(matrix m) {
    int i;
    for(i = 0; i < m.rows; ++i) free(m.data[i]);
    free(m.data);
}

static matrix augment_matrix(matrix m) {
    int i,j;
    matrix c = make_matrix(m.rows, m.cols*2);
    for(i = 0; i < m.rows; ++i){
        for(j = 0; j < m.cols; ++j){
            c.data[i][j] = m.data[i][j];
        }
    }
    for(j = 0; j < m.rows; ++j){
        c.data[j][j+m.cols] = 1;
    }
    return c;
}

static matrix make_identity(int rows, int cols) {
    int i;
    matrix m = make_matrix(rows, cols);
    for(i = 0; i < rows && i < cols; ++i){
        m.data[i][i] = 1;
    }
    return m;
}

static matrix matrix_mult_matrix(matrix a, matrix b) {
    assert(a.cols == b.rows);

    int i, j, k;
    matrix p = make_matrix(a.rows, b.cols);
    for(i = 0; i < p.rows; ++i){
        for(j = 0; j < p.cols; ++j){
            for(k = 0; k < a.cols; ++k){
                p.data[i][j] += a.data[i][k]*b.data[k][j];
            }
        }
    }
    return p;
}

static matrix matrix_sub_matrix(matrix a, matrix b) {
    assert(a.cols == b.cols);
    assert(a.rows == b.rows);
    int i, j;
    matrix p = make_matrix(a.rows, a.cols);
    for(i = 0; i < p.rows; ++i){
        for(j = 0; j < p.cols; ++j){
            p.data[i][j] = a.data[i][j] - b.data[i][j];
        }
    }
    return p;
}

static matrix transpose_matrix(matrix m) {
    matrix t;
    t.rows = m.cols;
    t.cols = m.rows;
    t.data = static_cast<double**>(calloc(size_t(t.rows), sizeof(double *)));
    int i, j;
    for(i = 0; i < t.rows; ++i){
        t.data[i] = static_cast<double*>(calloc(size_t(t.cols), sizeof(double)));
        for(j = 0; j < t.cols; ++j){
            t.data[i][j] = m.data[j][i];
        }
    }
    return t;
}

static double *matrix_mult_vector(matrix m, double *v) {
    double *p = static_cast<double*>(calloc(size_t(m.rows), sizeof(double)));
    int i, j;
    for(i = 0; i < m.rows; ++i){
        for(j = 0; j < m.cols; ++j){
            p[i] += m.data[i][j]*v[j];
        }
    }
    return p;
}

static matrix matrix_invert(matrix m) {
    //print_matrix(m);
    matrix none = {0,0,nullptr};
    if(m.rows != m.cols){
        fprintf(stderr, "Matrix not square\n");
        return none;
    }
    matrix c = augment_matrix(m);
    //print_matrix(c);

    int i, j, k;
    for(k = 0; k < c.rows; ++k){
        double p = 0.;
        int index = -1;
        for(i = k; i < c.rows; ++i){
            double val = fabs(c.data[i][k]);
            if(val > p){
                p = val;
                index = i;
            }
        }
        if(index == -1){
            fprintf(stderr, "Can't do it, sorry!\n");
            free_matrix(c);
            return none;
        }

        double *swap = c.data[index];
        c.data[index] = c.data[k];
        c.data[k] = swap;

        double val = c.data[k][k];
        c.data[k][k] = 1;
        for(j = k+1; j < c.cols; ++j){
            c.data[k][j] /= val;
        }
        for(i = k+1; i < c.rows; ++i){
            double s = -c.data[i][k];
            c.data[i][k] = 0;
            for(j = k+1; j < c.cols; ++j){
                c.data[i][j] +=  s*c.data[k][j];
            }
        }
    }
    for(k = c.rows-1; k > 0; --k){
        for(i = 0; i < k; ++i){
            double s = -c.data[i][k];
            c.data[i][k] = 0;
            for(j = k+1; j < c.cols; ++j){
                c.data[i][j] += s*c.data[k][j];
            }
        }
    }
    //print_matrix(c);
    matrix inv = make_matrix(m.rows, m.cols);
    for(i = 0; i < m.rows; ++i){
        for(j = 0; j < m.cols; ++j){
            inv.data[i][j] = c.data[i][j+m.cols];
        }
    }
    free_matrix(c);
    //print_matrix(inv);
    return inv;
}


static matrix solve_system(matrix M, matrix b) {
    matrix none = {0,0,nullptr};
    matrix Mt = transpose_matrix(M);
    matrix MtM = matrix_mult_matrix(Mt, M);
    matrix MtMinv = matrix_invert(MtM);
    if(!MtMinv.data) return none;
    matrix Mdag = matrix_mult_matrix(MtMinv, Mt);
    matrix a = matrix_mult_matrix(Mdag, b);
    free_matrix(Mt); free_matrix(MtM); free_matrix(MtMinv); free_matrix(Mdag);
    return a;
}


//
// Actual Testing code
//

using namespace smk;

Path testRoot();

template<typename T>
static bool same(const MatT<T> &a, const matrix &b)
{
    double const epsilon = 0.005;

    if (a.w != b.cols || a.h != b.rows || a.c != 1) {
        printf("Expected %d x %d x %d image, got %d x %d x %d\n", b.cols, b.rows, 1, a.w, a.h, a.c);
        return false;
    }

    for (int y = 0; y < a.h; ++y)
        for (int x = 0; x < a.w; ++x)
            if (!equivalent( double(a(y, x)), b.data[y][x], epsilon)) {
                printf("Mismatch (%d %d) %f %f\n", x, y, double(a(y, x)), b.data[y][x]);
                return false;
            }


    return true;
}

bool sameMat(const Mat &a, const Mat &b)
{
    float const epsilon = 0.005f;

    if (a.w != b.w || a.h != b.h || a.c != b.c)
    {
        printf("Expected %d x %d x %d image, got %d x %d x %d\n", b.w, b.h, b.c, a.w, a.h, a.c);
        return false;
    }

    for (int k = 0; k < a.c; ++k)
        for (int y = 0; y < a.h; ++y)
            for (int x = 0; x < a.w; ++x)
            {
                int index = a.w * a.h * k + a.w * y + x;
                if (!equivalent(a.data[index], b.data[index], epsilon))
                {
                    printf("Mismatch (%d %d %d) %f %f\n", x, y, k, double(a.data[index]), double(b.data[index]));
                    return false;
                }
            }

    return true;
}


static bool sameChannel(Mat const &a, Mat const &b, int const ac, int const bc)
{
    float const epsilon = 0.005f;

    if (a.w != b.w || a.h != b.h || a.c <= ac || b.c <= bc)
    {
        printf("Expected %d x %d x %d image, got %d x %d x %d\n", b.w, b.h, b.c, a.w, a.h, a.c);
        return false;
    }

    for (int y = 0; y < a.h; ++y)
        for (int x = 0; x < a.w; ++x)
        {
            int ai = a.w * a.h * ac + a.w * y + x;
            int bi = b.w * b.h * bc + b.w * y + x;
            if (!equivalent(a.data[ai], b.data[bi], epsilon))
            {
                printf("Mismatch (%d %d) %f %f\n", x, y, double(a.data[ai]), double(b.data[bi]));
                return false;
            }
        }

    return true;
}

static void randomize(Mat& a, matrix& b, int rows, int cols) {
    a = Mat(cols, rows);
    b = make_matrix(rows, cols);

    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < cols; ++j){
            float value = float(rand() % 100 - 50);
            b.data[i][j] = double(value);
            a(i,j) = value;
        }
    }
}


TEST_BEGIN(TestMatBasics)
{
    matrix t = make_identity(4, 3);
    TEST_ASSERT(same(Mat::makeIdentity(4, 3), t));
    free_matrix(t);

    t = make_identity(2, 3);
    TEST_ASSERT(same(Mat::makeIdentity(2, 3), t));
    free_matrix(t);

    t = make_identity(1, 3);
    TEST_ASSERT(same(Mat::makeIdentity(1, 3), t));
    free_matrix(t);

    t = make_identity(3, 3);
    TEST_ASSERT(same(Mat::makeIdentity(3, 3), t));
    free_matrix(t);

    t = make_identity(2, 2);
    TEST_ASSERT(same(Mat::makeIdentity(2, 2), t));
    free_matrix(t);

    t = make_identity_homography();
    TEST_ASSERT(same(Mat::makeIdentity3x3(), t));
    free_matrix(t);

    t = make_translation_homography(30.0f, 20.0f);
    TEST_ASSERT(same(Mat::makeTranslation3x3(30.0f, 20.0f), t));
    free_matrix(t);

    t =  make_translation_homography(120.0f, 40.0f);
    TEST_ASSERT(same(Mat::makeTranslation3x3(120.0f, 40.0f), t));
    free_matrix(t);

    {
        Mat m;
        matrix m1;
        randomize(m, m1, 10, 10);

        Matd md;
        //m.convert(md);
        //TEST_ASSERT(sameMat(m, md));

        md = m.convert<double>();

        free_matrix(m1);
    }


    for (int i = 0; i != 10; ++i) {
        Mat a;
        matrix b;
        randomize(a, b, 10, 10);

        matrix m = augment_matrix(b);
        TEST_ASSERT(same(a.augment(), m));
        free_matrix(m);
    }


    for (int i = 0; i != 10; ++i) {
        Mat a;
        matrix a1;
        randomize(a, a1, 10, 10);

        Mat b;
        matrix b1;
        randomize(b, b1, 10, 10);

        matrix m = matrix_mult_matrix(a1, b1);
        TEST_ASSERT(same(Mat::mmult(a, b), m));

        free_matrix(m);
        free_matrix(a1);
        free_matrix(b1);
    }


    for (int i = 0; i != 10; ++i) {
        Mat a;
        matrix a1;
        randomize(a, a1, 10, 10);

        Mat b;
        matrix b1;
        randomize(b, b1, 10, 10);

        matrix m = matrix_sub_matrix(a1, b1);
        TEST_ASSERT(same(Mat::sub(a, b), matrix_sub_matrix(a1, b1)));

        free_matrix(m);
        free_matrix(a1);
        free_matrix(b1);
    }

 
    for (int i = 0; i != 10; ++i) {
        Mat a;
        matrix a1;
        randomize(a, a1, 10, 10);

        Mat b;
        matrix b1;
        randomize(b, b1, 10, 10);

        matrix m = matrix_mult_matrix(a1, b1);
        TEST_ASSERT(same(Mat::mmult(a, b), matrix_mult_matrix(a1, b1)));

        free_matrix(m);
        free_matrix(a1);
        free_matrix(b1);
    }

    
    for (int i = 0; i != 10; ++i) {
        Mat a;
        matrix a1;
        randomize(a, a1, 3, 3);

        matrix m = transpose_matrix(a1);
        TEST_ASSERT(same(a.transpose(), m));
        free_matrix(m);
        free_matrix(a1);

        randomize(a, a1, 3, 2);
        m = transpose_matrix(a1);
        TEST_ASSERT(same(a.transpose(), m));
        free_matrix(m);
        free_matrix(a1);

        randomize(a, a1, 2, 3);
        m = transpose_matrix(a1);
        TEST_ASSERT(same(a.transpose(), m));
        free_matrix(m);
        free_matrix(a1);
    }


    for (int i = 0; i != 10; ++i) {
        Mat a;
        matrix a1;
        randomize(a, a1, 3, 3);

        Mat v;
        matrix v1;
        randomize(v, v1, 3, 1);

        double *pi = static_cast<double*>(calloc(size_t(v1.rows), sizeof(double)));
        for(int i = 0; i != v1.rows; ++i) {
            pi[i] = v1.data[i][0];
        }
        double *po = matrix_mult_vector(a1, pi);
        for(int i = 0; i != v1.rows; ++i) {
            v1.data[i][0] = po[i];
        }
        free(pi);
        free(po);

        TEST_ASSERT(same(Mat::vmult(a, v), v1));

        free_matrix(v1);
        free_matrix(a1);
    }
}
TEST_END(TestMatBasics)

TEST_BEGIN(TestMatInvert)
{
    for(int i = 0; i < 100; ++i){
        int s = rand() % 4 + 3;

        Mat m;
        matrix m1;
        randomize(m, m1, s, s);


        Mat inv = m.invert();
        matrix inv1 = matrix_invert(m1);
        TEST_ASSERT(same(inv, inv1));

        Mat res = Mat::mmult(m, inv);
        matrix res1 = matrix_mult_matrix(m1, inv1);
        TEST_ASSERT(same(res, res1));

        free_matrix(m1);
        free_matrix(inv1);
        free_matrix(res1);
    }
}
TEST_END(TestMatInvert)

TEST_BEGIN(TestMatProjMult)
{
    for (int i = 0; i < 100; ++i)
    {
        Mat m;
        matrix m1;
        randomize(m, m1, 3, 3);

        Mat p;
        matrix p1;
        randomize(p, p1, 3, 1);

        Mat res = Mat::mmult(m, p);
        matrix res1 = matrix_mult_matrix(m1, p1);
        TEST_ASSERT(same(res, res1));

        free_matrix(m1);
        free_matrix(res1);
    }
}
TEST_END(TestMatProjMult)

TEST_BEGIN(TestMatProjHomography)
{
    {
        // test project a very simple point translation
        Matches matches;
        Match match;

        match.p.x() = 1.0f;
        match.p.y() = 1.0f;
        matches.push_back(match);

        match.p.x() = 100.0f;
        match.p.y() = 1.0f;
        matches.push_back(match);

        match.p.x() = 100.0f;
        match.p.y() = 20.0f;
        matches.push_back(match);

        match.p.x() = 1.0f;
        match.p.y() = 20.0f;
        matches.push_back(match);
        for(Match& current : matches) {
            current.q = current.p;
            current.q.x() += 100.0f;
            current.q.y() += 100.0f;
        }

        Matd H = Matd::makeTranslation3x3(100, 100);
        int inliers = modelInliers(H, matches, 2.0f);
        TEST_ASSERT(inliers == 4);



        //
        // system solve
        //
        size_t n = matches.size();
        Matd M(8, int(n * 2));
        Matd b(1, int(n * 2));

        // fill in the matrices M and b.
        for (size_t i = 0; i < n; ++i)
        {
            double x = double(matches[i].p.x());
            double xp = double(matches[i].q.x());

            double y = double(matches[i].p.y());
            double yp = double(matches[i].q.y());

            int r = int(i * 2);
            M(r, 0) = x;
            M(r, 1) = y;
            M(r, 2) = 1;
            M(r, 3) = 0;
            M(r, 4) = 0;
            M(r, 5) = 0;
            M(r, 6) = -x * xp;
            M(r, 7) = -y * xp;
            b(r, 0) = xp;

            r++;

            M(r, 0) = 0;
            M(r, 1) = 0;
            M(r, 2) = 0;
            M(r, 3) = x;
            M(r, 4) = y;
            M(r, 5) = 1;
            M(r, 6) = -x * yp;
            M(r, 7) = -y * yp;
            b(r, 0) = yp;
        }

        matrix m1 = make_matrix(M.h, M.w);
        for (int y = 0; y != M.h; ++y)
            for (int x = 0; x != M.w; ++x)
                m1.data[y][x] = M(y,x);

        matrix b1 = make_matrix(b.h, b.w);
        for (int y = 0; y != b.h; ++y)
            for (int x = 0; x != b.w; ++x)
                b1.data[y][x] = b(y,x);

        TEST_ASSERT(same(M, m1));
        TEST_ASSERT(same(b, b1));

        matrix a1 = {0,0,nullptr};

        matrix Mt1 = transpose_matrix(m1);
        Matd Mt = M.transpose();
        TEST_ASSERT(same(M, m1));
        TEST_ASSERT(same(Mt, Mt1));

        matrix MtM1 = matrix_mult_matrix(Mt1, m1);
        Matd MtM = Matd::mmult(Mt, M);
        TEST_ASSERT(same(MtM, MtM1));

        matrix MtMinv1 = matrix_invert(MtM1);
        Matd MtMinv = MtM.invert();
        TEST_ASSERT(same(MtMinv, MtMinv1));

        matrix Mdag1 = {0,0,nullptr};
        if(MtMinv1.data) {
            Mdag1 = matrix_mult_matrix(MtMinv1, Mt1);
            Matd Mdag = Matd::mmult(MtMinv, Mt);
            TEST_ASSERT(same(Mdag, Mdag1));

            a1 = matrix_mult_matrix(Mdag1, b1);
            Matd a = Matd::mmult(Mdag, b);
            TEST_ASSERT(same(a, a1));


            Matd H(3, 3);

            H(0,0) = a(0,0);
            H(0,1) = a(1,0);
            H(0,2) = a(2,0);

            H(1,0) = a(3,0);
            H(1,1) = a(4,0);
            H(1,2) = a(5,0);

            H(2,0) = a(6,0);
            H(2,1) = a(7,0);
            H(2,2) = 1.0;

            int inliers = modelInliers(H, matches, 2.0f);
            TEST_ASSERT(inliers == 4);
        }
        free_matrix(Mt1); free_matrix(MtM1); free_matrix(MtMinv1); free_matrix(Mdag1);

        {
            matrix a1 = solve_system(m1, b1);
            Matd a = Matd::llsSolve(M, b);
            TEST_ASSERT(same(a, a1));
        }

        free_matrix(m1);
        free_matrix(b1);
        free_matrix(a1);
    }
}
TEST_END(TestMatProjHomography)

TEST_BEGIN(TestMaxCostAssignment)
{
    // http://dlib.net/max_cost_assignment.py.html

    // this simple example shows how to call dlib's optimal linear assignment
    // problem solver.  It is an implementation of the famous Hungarian algorithm
    // and is quite fast, operating in O(N^3) time.

    // Let's imagine you need to assign N people to N jobs.  Additionally, each
    // person will make your company a certain amount of money at each job, but each
    // person has different skills so they are better at some jobs and worse at
    // others.  You would like to find the best way to assign people to these jobs.
    // In particular, you would like to maximize the amount of money the group makes
    // as a whole.  This is an example of an assignment problem and is what is solved
    // by the dlib.max_cost_assignment() routine.

    // So in this example, let's imagine we have 3 people and 3 jobs. We represent
    // the amount of money each person will produce at each job with a cost matrix.
    // Each row corresponds to a person and each column corresponds to a job. So for
    // example, below we are saying that person 0 will make $1 at job 0, $2 at job 1,
    // and $6 at job 2.
    // cost = dlib.matrix([[1, 2, 6],
    //                     [5, 3, 6],
    //                     [4, 5, 0]])
    CostMatrix cost(3, 3);
    cost(0, 0) = 1;
    cost(0, 1) = 2;
    cost(0, 2) = 6;

    cost(1, 0) = 5;
    cost(1, 1) = 3;
    cost(1, 2) = 6;

    cost(2, 0) = 4;
    cost(2, 1) = 5;
    cost(2, 2) = 0;

    // To find out the best assignment of people to jobs we just need to call this
    // function.
    Assignment assignment = assignmentMaxCost(cost);

    // This prints optimal assignments:  [2, 0, 1]
    // which indicates that we should assign the person from the first row of the
    // cost matrix to job 2, the middle row person to job 0, and the bottom row
    // person to job 1.
    TEST_ASSERT(assignment[0] == 2);
    TEST_ASSERT(assignment[1] == 0);
    TEST_ASSERT(assignment[2] == 1);

    // This prints optimal cost:  16.0
    // which is correct since our optimal assignment is 6+5+5.
    TEST_ASSERT(assignmentCost(cost, assignment) == 16);
}
TEST_END(TestMaxCostAssignment)


TEST_BEGIN(TestGetPixel)
{
    Mat im = loadImage(mergePaths(testRoot(), U"data/vision/dots.png"));
    // Test within image
    TEST_ASSERT(equivalent(0.0f, im.get(0,0,0)));
    TEST_ASSERT(equivalent(1.0f, im.get(1,0,1)));
    TEST_ASSERT(equivalent(0.0f, im.get(2,0,1)));

    // Test padding
    TEST_ASSERT(equivalent(1.0f, im.getClamp(0,3,1)));
    TEST_ASSERT(equivalent(1.0f, im.getClamp(7,8,0)));
    TEST_ASSERT(equivalent(0.0f, im.getClamp(7,8,1)));
    TEST_ASSERT(equivalent(1.0f, im.getClamp(7,8,2)));
}
TEST_END(TestGetPixel)

TEST_BEGIN(TestSetPixel)
{
    Mat im = loadImage(mergePaths(testRoot(), U"data/vision/dots.png"));
    Mat d = Mat(4,2,4);

    d.set(0,0,0,0).set(0,0,1,0).set(0,0,2,0).set(0,0,3,1);
    d.set(1,0,0,1).set(1,0,1,1).set(1,0,2,1).set(1,0,3,1);
    d.set(2,0,0,1).set(2,0,1,0).set(2,0,2,0).set(2,0,3,1);
    d.set(3,0,0,1).set(3,0,1,1).set(3,0,2,0).set(3,0,3,1);

    d.set(0,1,0,0).set(0,1,1,1).set(0,1,2,0).set(0,1,3,1);
    d.set(1,1,0,0).set(1,1,1,1).set(1,1,2,1).set(1,1,3,1);
    d.set(2,1,0,0).set(2,1,1,0).set(2,1,2,1).set(2,1,3,1);
    d.set(3,1,0,1).set(3,1,1,0).set(3,1,2,1).set(3,1,3,1);

    // Test images are same
    TEST_ASSERT(sameMat(im, d));
}
TEST_END(TestSetPixel)

TEST_BEGIN(TestPixelCopy)
{
    Mat im = loadImage(mergePaths(testRoot(), U"data/vision/dog.jpg"));
    Mat c = im.clone();
    TEST_ASSERT(sameMat(im, c));
}
TEST_END(TestPixelCopy)

TEST_BEGIN(TestPixelShift)
{
    Mat im = loadImage(mergePaths(testRoot(), U"data/vision/dog.jpg"));
    Mat c = im.clone();
    c.add(1, 0.1f);

    TEST_ASSERT(equivalent(im.data[0], c.data[0]));
    TEST_ASSERT(equivalent(im.data[im.w*im.h+13] + .1f,  c.data[im.w*im.h + 13]));
    TEST_ASSERT(equivalent(im.data[2*im.w*im.h+72],  c.data[2*im.w*im.h + 72]));
    TEST_ASSERT(equivalent(im.data[im.w*im.h+47] + .1f,  c.data[im.w*im.h + 47]));
}
TEST_END(TestPixelShift)

TEST_BEGIN(TestPixelGrayscale)
{
    Mat im = loadImage(mergePaths(testRoot(), U"data/vision/colorbar.png"));
    Mat gray = rgb2gray(im);
    Mat g = loadImage(mergePaths(testRoot(), U"data/vision/colorbar_gray.png"));

    TEST_ASSERT(sameMat(gray, g));
}
TEST_END(TestPixelGrayscale)

TEST_BEGIN(TestPixelRgbToHsv)
{
    Mat im = loadImage(mergePaths(testRoot(), U"data/vision/dog.jpg"));
    rgb2hsvInplace(im);

    Mat hsv = loadImage(mergePaths(testRoot(), U"data/vision/dog.hsv.png"));
    TEST_ASSERT(sameMat(im, hsv));
}
TEST_END(TestPixelRgbToHsv)

TEST_BEGIN(TestPixelHsvToRgb)
{
    Mat im = loadImage(mergePaths(testRoot(), U"data/vision/dog.jpg"));
    Mat c = im.clone();
    rgb2hsvInplace(im);
    hsv2rgbInplace(im);
    TEST_ASSERT(sameMat(im, c));
}
TEST_END(TestPixelHsvToRgb)

TEST_BEGIN(TestThreshold)
{
    Mat loaded = loadImage(mergePaths(testRoot(), U"data/vision/gradient.png"));
    Mat loaded_gray = rgb2gray(loaded);

    {
        Mat gray = loaded_gray.clone();
        threshold(gray, gray, ThresholdMode::Binary, 0.5f);
        Mat reference = loadImage(mergePaths(testRoot(), U"data/vision/gradient_threshold_binary.png"));
        TEST_ASSERT(sameMat(reference, gray));
    }

    {
        Mat gray = loaded_gray.clone();
        threshold(gray, gray, ThresholdMode::BinaryInverted, 0.5f);
        Mat reference = loadImage(mergePaths(testRoot(), U"data/vision/gradient_threshold_binary_inverted.png"));
        TEST_ASSERT(sameMat(reference, gray));
    }

    {
        Mat gray = loaded_gray.clone();
        threshold(gray, gray, ThresholdMode::Truncate, 0.5f, 0.5f);
        Mat reference = loadImage(mergePaths(testRoot(), U"data/vision/gradient_threshold_truncate.png"));
        TEST_ASSERT(sameMat(reference, gray));
    }

    {
        Mat gray = loaded_gray.clone();
        threshold(gray, gray, ThresholdMode::ToZero, 0.5f);
        Mat reference = loadImage(mergePaths(testRoot(), U"data/vision/gradient_threshold_to_zero.png"));
        TEST_ASSERT(sameMat(reference, gray));
    }

    {
        Mat gray = loaded_gray.clone();
        threshold(gray, gray, ThresholdMode::ToZeroInverted, 0.5f);
        Mat reference = loadImage(mergePaths(testRoot(), U"data/vision/gradient_threshold_to_zero_inverted.png"));
        TEST_ASSERT(sameMat(reference, gray));
    }
}
TEST_END(TestThreshold)

TEST_BEGIN(TestThresholdOtsu)
{
    Mat gray = loadImage(mergePaths(testRoot(), U"data/vision/box.png"), 1);
    float otsu = thresholdOtsu(gray, gray, ThresholdMode::Binary);
    Mat reference = loadImage(mergePaths(testRoot(), U"data/vision/threshold_otsu.png"));
    TEST_ASSERT(sameMat(reference, gray));
}
TEST_END(TestThresholdOtsu)

TEST_BEGIN(TestIntegralImages)
{
    //https://en.wikipedia.org/wiki/Summed-area_table
    Mat a(6,6);
    a(0, 0) = 31;
    a(0, 1) = 2;
    a(0, 2) = 4;
    a(0, 3) = 33;
    a(0, 4) = 5;
    a(0, 5) = 36;

    a(1, 0) = 12;
    a(1, 1) = 26;
    a(1, 2) = 9;
    a(1, 3) = 10;
    a(1, 4) = 29;
    a(1, 5) = 25;

    a(2, 0) = 13;
    a(2, 1) = 17;
    a(2, 2) = 21;
    a(2, 3) = 22;
    a(2, 4) = 20;
    a(2, 5) = 18;

    a(3, 0) = 24;
    a(3, 1) = 23;
    a(3, 2) = 15;
    a(3, 3) = 16;
    a(3, 4) = 14;
    a(3, 5) = 19;

    a(4, 0) = 30;
    a(4, 1) = 8;
    a(4, 2) = 28;
    a(4, 3) = 27;
    a(4, 4) = 11;
    a(4, 5) = 7;

    a(5, 0) = 1;
    a(5, 1) = 35;
    a(5, 2) = 34;
    a(5, 3) = 3;
    a(5, 4) = 32;
    a(5, 5) = 6;


    Mat i(6,6);
    i(0, 0) = 31;
    i(0, 1) = 33;
    i(0, 2) = 37;
    i(0, 3) = 70;
    i(0, 4) = 75;
    i(0, 5) = 111;

    i(1, 0) = 43;
    i(1, 1) = 71;
    i(1, 2) = 84;
    i(1, 3) = 127;
    i(1, 4) = 161;
    i(1, 5) = 222;

    i(2, 0) = 56;
    i(2, 1) = 101;
    i(2, 2) = 135;
    i(2, 3) = 200;
    i(2, 4) = 254;
    i(2, 5) = 333;

    i(3, 0) = 80;
    i(3, 1) = 148;
    i(3, 2) = 197;
    i(3, 3) = 278;
    i(3, 4) = 346;
    i(3, 5) = 444;

    i(4, 0) = 110;
    i(4, 1) = 186;
    i(4, 2) = 263;
    i(4, 3) = 371;
    i(4, 4) = 450;
    i(4, 5) = 555;

    i(5, 0) = 111;
    i(5, 1) = 222;
    i(5, 2) = 333;
    i(5, 3) = 444;
    i(5, 4) = 555;
    i(5, 5) = 666;


    Mat ai;
    makeIntegralImage(a, ai);
    TEST_ASSERT(sameMat(ai, i));
}
TEST_END(TestIntegralImages)

TEST_BEGIN(TestBoxFilter)
{
    Mat im = loadImage(mergePaths(testRoot(), U"data/vision/dog.jpg"));
    Mat imi;
    makeIntegralImage(im, imi);

    Mat blur;
    boxfilterIntegralImage(imi, 7, blur);
    blur.clamp();
    //saveImage("dog-box7_integral.png", blur);

    Mat gt = loadImage(mergePaths(testRoot(), U"data/vision/dog-box7_integral.png"));
    TEST_ASSERT(sameMat(blur, gt));
}
TEST_END(TestBoxFilter)

TEST_BEGIN(TestOpticalflow)
{
    LucasKanade lk;

    Mat a = loadImage(mergePaths(testRoot(), U"data/vision/dog_a.jpg"));
    Mat b = loadImage(mergePaths(testRoot(), U"data/vision/dog_b.jpg"));
    Mat flow;
    lk.opticalflow(b, a, 15, 8, flow);
    drawFlow(a, flow, 8);
//    saveImage("dump.png", a);

//   Mat loaded = loadImage(mergePaths(testRoot(), U"data/vision/opticalflow_lines.png");
//    TEST_ASSERT(sameMat(a, loaded));
}
TEST_END(TestOpticalflow)


TEST_BEGIN(TestResizeNN)
{
    {
        Mat im = loadImage(mergePaths(testRoot(), U"data/vision/dogsmall.jpg"));
        Mat resized = resize(im, im.w*4, im.h*4, ResizeMode::NearestNeighbor);
        Mat gt = loadImage(mergePaths(testRoot(), U"data/vision/dog4x-nn-for-test.png"));
        TEST_ASSERT(sameMat(resized, gt));
    }

    {
        Mat im = loadImage(mergePaths(testRoot(), U"data/vision/dog.jpg"));
        Mat resized = resize(im, 713, 467, ResizeMode::NearestNeighbor);
        Mat gt = loadImage(mergePaths(testRoot(), U"data/vision/dog-resize-nn.png"));
        TEST_ASSERT(sameMat(resized, gt));
    }
}
TEST_END(TestResizeNN)

TEST_BEGIN(TestResizeBL)
{
    {
        Mat im = loadImage(mergePaths(testRoot(), U"data/vision/dogsmall.jpg"));
        Mat resized = resize(im, im.w*4, im.h*4, ResizeMode::Bilinear);
        Mat gt = loadImage(mergePaths(testRoot(), U"data/vision/dog4x-bl.png"));
        TEST_ASSERT(sameMat(resized, gt));
    }

    {
        Mat im = loadImage(mergePaths(testRoot(), U"data/vision/dog.jpg"));
        Mat resized = resize(im, 713, 467, ResizeMode::Bilinear);
        Mat gt = loadImage(mergePaths(testRoot(), U"data/vision/dog-resize-bil.png"));
        TEST_ASSERT(sameMat(resized, gt));
    }
}
TEST_END(TestResizeBL)

TEST_BEGIN(TestResizeMulti)
{
    Mat im = loadImage(mergePaths(testRoot(), U"data/vision/dog.jpg"));

    for (int i = 0; i < 10; i++){
        Mat im1 = resize(im, im.w*4, im.h*4, ResizeMode::Bilinear);
        Mat im2 = resize(im1, im1.w/4, im1.h/4, ResizeMode::Bilinear);
        im = im2;
    }
    Mat gt = loadImage(mergePaths(testRoot(), U"data/vision/dog-multipleresize.png"));
    TEST_ASSERT(sameMat(im, gt));
}
TEST_END(TestResizeMulti)


TEST_BEGIN(TestHighPass)
{
    Mat im = loadImage(mergePaths(testRoot(), U"data/vision/dog.jpg"));
    Mat f = makeHighpassFilter();
    Mat blur = convolve(im, f, false);
    blur.clamp();
    Mat gt = loadImage(mergePaths(testRoot(), U"data/vision/dog-highpass.png"));
    TEST_ASSERT(sameMat(blur, gt));
}
TEST_END(TestHighPass)

TEST_BEGIN(TestEmboss)
{
    Mat im = loadImage(mergePaths(testRoot(), U"data/vision/dog.jpg"));
    Mat f = makeEmbossFilter();
    Mat blur = convolve(im, f);
    blur.clamp();

    Mat gt = loadImage(mergePaths(testRoot(), U"data/vision/dog-emboss.png"));
    TEST_ASSERT(sameMat(blur, gt));
}
TEST_END(TestEmboss)

TEST_BEGIN(TestSharpen)
{
    Mat im = loadImage(mergePaths(testRoot(), U"data/vision/dog.jpg"));
    Mat f = makeSharpenFilter();
    Mat blur = convolve(im, f);
    blur.clamp();

    Mat gt = loadImage(mergePaths(testRoot(), U"data/vision/dog-sharpen.png"));
    TEST_ASSERT(sameMat(blur, gt));
}
TEST_END(TestSharpen)

TEST_BEGIN(TestConvolution)
{
    {
        Mat filter(4, 4, 1);
        filter.fill(4.0);
        filter.l1Normalize();
        TEST_ASSERT(equivalent(filter.get(0), 4.0f / (4 * 4 * 4)));
        TEST_ASSERT(equivalent(filter.sum(0), 1.0f));
    }

    {
        Mat filter = makeBoxFilter(7);
        TEST_ASSERT(equivalent(filter.get(0), 1.0f / (7 * 7)));
        TEST_ASSERT(equivalent(filter.sum(0), 1.0f));
    }


    Mat im = loadImage(mergePaths(testRoot(), U"data/vision/dog.jpg"));
    Mat f = makeBoxFilter(7);
    Mat blur = convolve(im, f);
    blur.clamp();

    Mat gt = loadImage(mergePaths(testRoot(), U"data/vision/dog-box7.png"));
    TEST_ASSERT(sameMat(blur, gt));
}
TEST_END(TestConvolution)

TEST_BEGIN(TestGaussianFilter) 
{
    Mat f = makeGaussianFilter(7.0f);

    for(int i = 0; i < f.w * f.h * f.c; i++){
        f.data[i] *= 100;
    }

    Mat gt = loadImage(mergePaths(testRoot(), U"data/vision/gaussian_filter_7.png"));
    TEST_ASSERT(sameMat(f, gt));
}
TEST_END(TestGaussianFilter)

TEST_BEGIN(TestGaussianBlur) 
{
    Mat im = loadImage(mergePaths(testRoot(), U"data/vision/dog.jpg"));
    Mat f = makeGaussianFilter(2.0f);
    //f.l1Normalize(); // blur should be normalized [0.0f, 1.0f] but the kernel is already normalized
    Mat blur = convolve(im, f);
    blur.clamp();

    Mat gt = loadImage(mergePaths(testRoot(), U"data/vision/dog-gauss2.png"));
    TEST_ASSERT(sameMat(blur, gt));

    blur = smoothImage(im, 2.0f);
    blur.clamp();
    TEST_ASSERT(sameMat(blur, gt));


    Mat c0(im.w, im.h, 1);
    c0.fill(im, 0, 0);

    Mat c1(im.w, im.h, 1);
    c1.fill(im, 1, 0);

    Mat c2(im.w, im.h, 1);
    c2.fill(im, 2, 0);

    Mat d(im.w, im.h, 3);
    Mat d0(im.w, im.h, 1, d.data + (0 * im.w * im.h));
    Mat d1(im.w, im.h, 1, d.data + (1 * im.w * im.h));
    Mat d2(im.w, im.h, 1, d.data + (2 * im.w * im.h));

    smoothImage(c0, d0, 2.0f);
    smoothImage(c1, d1, 2.0f);
    smoothImage(c2, d2, 2.0f);

    Mat D(im.w, im.h, 3);
    smoothImage(im, D, 2.0f);

    TEST_ASSERT(sameMat(D, d));
}
TEST_END(TestGaussianBlur)

TEST_BEGIN(TestHybridImage) 
{
    Mat man = loadImage(mergePaths(testRoot(), U"data/vision/melisa.png"), 3);
    Mat woman = loadImage(mergePaths(testRoot(), U"data/vision/aria.png"), 3);
    Mat f = makeGaussianFilter(2.0f);
    Mat lfreq_man = convolve(man, f);
    Mat lfreq_w = convolve(woman, f);
    Mat hfreq_w = Mat::sub(woman , lfreq_w);
    Mat reconstruct = Mat::add(lfreq_man , hfreq_w);
    Mat gt = loadImage(mergePaths(testRoot(), U"data/vision/hybrid.png"));
    reconstruct.clamp();
    TEST_ASSERT(sameMat(reconstruct, gt));
}
TEST_END(TestHybridImage)

TEST_BEGIN(TestFrequencyImage)
{
    Mat im = loadImage(mergePaths(testRoot(), U"data/vision/dog.jpg"));
    Mat f = makeGaussianFilter(2.0f);
    Mat lfreq = convolve(im, f);
    Mat hfreq = Mat::sub(im, lfreq);
    Mat reconstruct = Mat::add(lfreq , hfreq);

    Mat low_freq = loadImage(mergePaths(testRoot(), U"data/vision/low-frequency.png"));
    Mat high_freq = loadImage(mergePaths(testRoot(), U"data/vision/high-frequency-clamp.png"));

    lfreq.clamp();
    hfreq.clamp();
    TEST_ASSERT(sameMat(lfreq, low_freq));
    TEST_ASSERT(sameMat(hfreq, high_freq));
    TEST_ASSERT(sameMat(reconstruct, im));
}
TEST_END(TestFrequencyImage)

TEST_BEGIN(TestGradients) 
{
    Mat gray;
    Mat im = loadImage(mergePaths(testRoot(), U"data/vision/dog.jpg"));
    rgb2gray(im, gray);

    Mat gx, gy;
    gradientSingleChannel(gray, gx, gy);

    Mat gx1, gy1;
    gradient(gray, gx1, gy1);

    TEST_ASSERT(sameMat(gx, gx1));
    TEST_ASSERT(sameMat(gy, gy1));
}
TEST_END(TestGradients)

TEST_BEGIN(TestSobel) 
{
    Mat im = loadImage(mergePaths(testRoot(), U"data/vision/dog.jpg"));
    Mat mag;
    Mat theta;
    gradientMagnitudeAngle(im, mag, theta);

    mag.featureNormalize();
    theta.featureNormalize();

    Mat gt_mag = loadImage(mergePaths(testRoot(), U"data/vision/magnitude.png"));
    Mat gt_theta = loadImage(mergePaths(testRoot(), U"data/vision/theta.png"));
    TEST_ASSERT(gt_mag.w == mag.w && gt_theta.w == theta.w);
    TEST_ASSERT(gt_mag.h == mag.h && gt_theta.h == theta.h);
    TEST_ASSERT(gt_mag.c == mag.c && gt_theta.c == theta.c);

    if( gt_mag.w != mag.w || gt_theta.w != theta.w ||
        gt_mag.h != mag.h || gt_theta.h != theta.h ||
        gt_mag.c != mag.c || gt_theta.c != theta.c ) return;

    int i;
    for(i = 0; i < gt_mag.w*gt_mag.h; ++i){
        if(equivalent(gt_mag.data[i], 0.0f)){
            gt_theta.data[i] = 0;
            theta.data[i] = 0;
        }
        if(equivalent(gt_theta.data[i], 0.0f) || equivalent(gt_theta.data[i], 1.0f)){
            gt_theta.data[i] = 0;
            theta.data[i] = 0;
        }
    }

    TEST_ASSERT(sameMat(mag, gt_mag));
    TEST_ASSERT(sameMat(theta, gt_theta));
}
TEST_END(TestSobel)

TEST_BEGIN(TestSobelColor)
{
    Mat im = loadImage(mergePaths(testRoot(), U"data/vision/dog.jpg"));
    Mat mag, theta;
    gradientMagnitudeAngle(im, mag, theta);

    mag.featureNormalize();
    theta.featureNormalize();

    Mat color(im.w, im.h, 3);
    color.fill(theta, 0, 0);
    color.fill(mag, 0, 1);
    color.fill(mag, 0, 2);

    hsv2rgbInplace(color);

    //saveImage((mergePaths(testRoot(), U"data/vision/sobel_color1.png")), color);

    Mat sobel = loadImage(mergePaths(testRoot(), U"data/vision/sobel_color.png"));
    //TEST_ASSERT(sameMat(sobel, color));
}
TEST_END(TestSobelColor)

TEST_BEGIN(TestCanny) 
{
    Mat im = loadImage(mergePaths(testRoot(), U"data/vision/Lenna.png"));
    Mat gray = rgb2gray(im);

    Mat canny;
    smk::canny(gray, canny,  0.10f, 0.50f, 0.8f);

    //saveImage("canny.png", canny);

    Mat lcanny = loadImage(mergePaths(testRoot(), U"data/vision/lenna_canny.png"));
    TEST_ASSERT(sameMat(canny, lcanny));
}
TEST_END(TestCanny)

TEST_BEGIN(TestExtractImage4Points) 
{
    Mat im = loadImage(mergePaths(testRoot(), U"data/vision/fireframe.png"));

    int mm = 10;
    Mat frame(28 * mm, 48 * mm, im.c);

    std::array<Vector2F, 4> points;
    points[0] = Vector2F(227, 169);
    points[1] = Vector2F(459, 763);
    points[2] = Vector2F(217, 667);
    points[3] = Vector2F(554, 208);
    extractImage4points(im, frame, points);

    //saveImage("warp.png", frame);

    Mat warped = loadImage(mergePaths(testRoot(), U"data/vision/fireframe_warped.png"));
    TEST_ASSERT(sameMat(frame, warped));
}
TEST_END(TestExtractImage4Points)

TEST_BEGIN(TestFilter) 
{
    Mat im = loadImage(mergePaths(testRoot(), U"data/vision/Rainier1.png"), 3);

    float const sigma = 2.0f;
    int size = im.w * im.h;

    Mat I(im.w, im.h, 3);
    Mat IxIx = I.channelView(0);
    Mat IyIy = I.channelView(1);
    Mat IxIy = I.channelView(2);
    gradient(im, IxIx, IyIy);

    TEST_ASSERT(sameChannel(I, IxIx, 0, 0));
    TEST_ASSERT(sameChannel(I, IyIy, 1, 0));
    TEST_ASSERT(sameChannel(I, IxIy, 2, 0));
    

    float x, y;
    for (int i = 0; i != size; ++i)
    {
        x = IxIx.data[i];
        y = IyIy.data[i];

        IxIy.data[i] = x * y;
        IxIx.data[i] = x * x;
        IyIy.data[i] = y * y;
    }

    TEST_ASSERT(sameChannel(I, IxIx, 0, 0));
    TEST_ASSERT(sameChannel(I, IyIy, 1, 0));
    TEST_ASSERT(sameChannel(I, IxIy, 2, 0));

    Mat S;
    S.reshape(im.w, im.h, 3);
    Mat Sxx(im.w, im.h, 1, S.data + (0 * size));
    Mat Syy(im.w, im.h, 1, S.data + (1 * size));
    Mat Sxy(im.w, im.h, 1, S.data + (2 * size));
    smoothImage(IxIx, Sxx, sigma);
    smoothImage(IyIy, Syy, sigma);
    smoothImage(IxIy, Sxy, sigma);

    TEST_ASSERT(sameChannel(S, Sxx, 0, 0));
    TEST_ASSERT(sameChannel(S, Syy, 1, 0));
    TEST_ASSERT(sameChannel(S, Sxy, 2, 0));

    Mat S1;
    S.reshape(im.w, im.h, 3);
    smoothImage(I, S1, 2.0f);
    TEST_ASSERT(sameMat(S, S1));
}
TEST_END(TestFilter)

TEST_BEGIN(TestDrawHarris) 
{
    Mat im = loadImage(mergePaths(testRoot(), U"data/vision/Rainier1.png"), 3);

    Mat S;
    harrisStructureMatrix(im, S, 2.0f);
    TEST_ASSERT(S.w == im.w && S.h == im.h && S.c == 3);

    drawHarrisCorners(im, 2.0f, 50.0f, 3);
    //saveImage("harris_corners.png", im);

    Mat result = loadImage(mergePaths(testRoot(), U"data/vision/harris_corners.png"));
    TEST_ASSERT(sameMat(im, result));
}
TEST_END(TestDrawHarris)

TEST_BEGIN(TestDrawMatches) 
{
    Mat a = loadImage(mergePaths(testRoot(), U"data/vision/Rainier1.png"), 3);
    Mat b = loadImage(mergePaths(testRoot(), U"data/vision/Rainier2.png"), 3);

    Mat out = drawMatches(a, b, 2.0f, 50.0f, 3);
    //saveImage("harris_matches.png", out);

    Mat result = loadImage(mergePaths(testRoot(), U"data/vision/harris_matches.png"));
    TEST_ASSERT(sameMat(out, result));
}
TEST_END(TestDrawMatches)

TEST_BEGIN(TestHomography) 
{
    // test project points
    {
        Matches matches;
        Match match;

        match.p.x() = 0.0f;
        match.p.y() = 0.0f;
        matches.push_back(match);

        match.p.x() = 100.0f;
        match.p.y() = 0.0f;
        matches.push_back(match);

        match.p.x() = 100.0f;
        match.p.y() = 20.0f;
        matches.push_back(match);

        match.p.x() = 0.0f;
        match.p.y() = 20.0f;
        matches.push_back(match);
        for(Match& current : matches) {
            current.q = current.p;
            current.q.x() += 100.0f;
            current.q.y() += 100.0f;
        }

        Matd H = Matd::makeTranslation3x3(100, 100);
        int inliers = modelInliers(H, matches, 2.0f);
        TEST_ASSERT(inliers == 4);


    }


    {
        Matches matches;
        Match match;

        match.p.x() = 0.0f;
        match.p.y() = 0.0f;
        matches.push_back(match);

        match.p.x() = 100.0f;
        match.p.y() = 0.0f;
        matches.push_back(match);

        match.p.x() = 100.0f;
        match.p.y() = 20.0f;
        matches.push_back(match);

        match.p.x() = 0.0f;
        match.p.y() = 20.0f;
        matches.push_back(match);

        for(Match& current : matches) {
            current.q = current.p;
            current.q.x() += 100.0f;
            current.q.y() += 100.0f;
        }

        Matd H = computeHomography(matches);
        assert(H.size() > 0);

        int inliers = modelInliers(H, matches, 2.0f);
        TEST_ASSERT(inliers == 4);
    }
}
TEST_END(TestHomography)

TEST_BEGIN(TestRansac) 
{
    srand(10);

    Mat a = loadImage(mergePaths(testRoot(), U"data/vision/Rainier1.png"), 3);
    Mat b = loadImage(mergePaths(testRoot(), U"data/vision/Rainier2.png"), 3);

    float sigma = 2.0;
    float thresh = 50.0f;
    int nms = 3;

    // Calculate corners and descriptors
    Descriptors ad = harrisCornerDetector(a, sigma, thresh, nms);
    Descriptors bd = harrisCornerDetector(b, sigma, thresh, nms);

    // Find matches
    Matches m = matchDescriptors(ad, bd);

    float inlier_thresh = 2.0f;
    int iters = 10000;
    int cutoff = 30;

    // Run RANSAC to find the homography
    Matd H = RANSAC(m, inlier_thresh, iters, cutoff);

    // Mark corners and matches between images
    markCorners(a, ad);
    markCorners(b, bd);
    Mat inlier_matches = drawInliers(a, b, H, m, inlier_thresh);
    //saveImage("ransac_inliers.png", inlier_matches);

    Mat result = loadImage(mergePaths(testRoot(), U"data/vision/ransac_inliers.png"));
    TEST_ASSERT(sameMat(inlier_matches, result));
}
TEST_END(TestRansac)