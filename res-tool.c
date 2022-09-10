#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define HELP_INFO_COUNT 10
#define RTN_PARA_ERROR __DBL_MIN_10_EXP__
#define RTN_PARA_INVALID __DBL_MIN_10_EXP__
#define CalcFormula(Vr, Vo, r1) ((r1) / (((Vo) / (Vr)) - 1))

const char helpMenu[HELP_INFO_COUNT][128] = {
    "-h    --help          show help menu, add another param for more info \n",
    "-d    --stepdown      calculate res value under DC/DC step-down mode \n",
    "-p    --parallel      calculate res under parallel mode \n",
    // there is an empty line
    "\n-----------------  other command have not been set  ----------------- \n",
};

const char stepDownHelpInfo[] = "Formula: V-out = (1 + R1 / R2) * V-ref \ne.g. | input  <-d>  <V-ref>  <V-out>  <R1>";
const char parallelHelpInfo[] = "Formula: Rp = (R1 * R2) / (R1 + R2) \ne.g. | input  <-p> <mode>  <Rp/R2>  <R1>\nNOTICE: Rp != 0, R1 !=0, Rp < R1 \n";

typedef enum ChipMode_e
{
    MODE_DCDC_STEP_DOWN = 1,
    MODE_PARALLEL_RES_1 = 2,
    MODE_PARALLEL_RES_2 = 3
} ChipMode_t;

bool isstring(char *arg)
{
    if (isdigit(arg[0]))
    {
        return 0;
    }

    return 1;
}

double stepDown_R2Calc(char **param, unsigned int param_n)
{

    double Vref = 0;
    double Vout = 0;
    double R1;
    double R2;

    if (param_n < 5)
    {
        printf("input parameters is not enough! \r\n");
    }
    else
    {
        printf("calculating R2 in DC/DC step-down mode... \n");

        Vref = atof(param[2]);
        if ((Vref > -1e-10) && (Vref < 1e-10))
        {
            printf("invalid input param <V-ref>! \n");
            return RTN_PARA_ERROR;
        }

        Vout = atof(param[3]);
        if ((Vout > -1e-10) && (Vout < 1e-10))
        {
            printf("invalid input param <V-out>! \n");
            return RTN_PARA_ERROR;
        }

        R1 = atof(param[4]);
        if ((R1 > -1e-10) && (R1 < 1e-10))
        {
            printf("invalid input param <R1>! \n");
            return RTN_PARA_ERROR;
        }

        R2 = CalcFormula(Vref, Vout, R1);

        printf("R2 = %lf \n", R2);

        if (param_n > 5)
        {
            printf("other input parameters are ignored! \n");
        }
    }

    return R2;
}

double parallel_RCalc(char **param, unsigned int param_n, ChipMode_t mode)
{
    double Rp, R1, R2;
    if (param_n < 5)
    {
        printf("input parameters is not enough! \r\n");
    }
    else
    {
        if (mode == MODE_PARALLEL_RES_1)
        {
            Rp = atof(param[3]);
            R1 = atof(param[4]);
            if (Rp == 0 || R1 == 0 || Rp == R1 || Rp > R1)
            {
                printf("invalid input parameters \n");
                return RTN_PARA_INVALID;
            }

            R2 = (Rp * R1) / (R1 - Rp);
            printf("anthor res value is %lf \n", R2);
        }
        else
        {
            R2 = atof(param[3]);
            R1 = atof(param[4]);
            if (R2 == 0 && R1 == 0)
            {
                Rp == 0;
                printf("after paralleled, total res value is 0 \n");
                return 0;
            }

            Rp = (R1 * R2) / (R1 + R2);
            printf("after paralleled, total res value is %lf \n", Rp);
        }

        if (param_n > 5)
        {
            printf("other input parameters are ignored! \n");
        }
    }
}

int main(char *argv, char **args)
{
    char **param = args;
    int param_n = 1;

    ChipMode_t mode = 0;

    if (param[param_n] == NULL)
    {
        printf("no parameters! \n");
        return 0;
    }
    else if (!strcmp(param[param_n], "-h") || !strcmp(param[param_n], "--help"))
    {
        if (param[param_n + 1] == NULL)
        {
            for (int j = 0; j < HELP_INFO_COUNT; j++)
                printf("%s", helpMenu[j]);
            return 0;
        }
        else if (!strcmp(param[param_n + 1], "-d"))
        {
            printf("%s \n", stepDownHelpInfo);
            return 0;
        }
        else if (!strcmp(param[param_n + 1], "-p"))
        {
            printf("%s \n", parallelHelpInfo);
            return 0;
        }
        else
        {
        }
    }
    else if (!strcmp(param[param_n], "-d"))
    {
        while (param[param_n] != NULL)
        {
            if (isstring(param[param_n]) && param_n != 1)
            {
                printf("error input parameters! \n");
                return 0;
            }
            param_n++;
        }

        mode = MODE_DCDC_STEP_DOWN;
    }
    else if (!strcmp(param[param_n], "-p"))
    {

        if (atoi(param[2]) == 1)
        {
            mode = MODE_PARALLEL_RES_1;
        }
        else if (atoi(param[2]) == 2)
        {
            mode = MODE_PARALLEL_RES_2;
        }
        else
        {
            printf("error input parameters! \n");
            return 0;
        }

        while (param[param_n] != NULL)
        {
            if (isstring(param[param_n]) && param_n != 1)
            {
                printf("error input parameters! \n");
                return 0;
            }

            param_n++;
        }
    }

    switch (mode)
    {
    case MODE_DCDC_STEP_DOWN:

        stepDown_R2Calc(param, param_n);

        break;

    case MODE_PARALLEL_RES_1:
    case MODE_PARALLEL_RES_2:

        parallel_RCalc(param, param_n, mode);

        break;

    default:
        break;
    }

    return 0;
}