#ifndef DEDX_WS
#define DEDX_WS

#define _DEDX_MAXELEMENTS 150
#define _DEDX_DATA_FOLDER "data/"

typedef struct
{
    int cache;
    int hits;
    int miss;
} _dedx_lookup_accelerator;

typedef struct
{
    float a;
    float b;
    float c;
    float d;
    float x;
} _dedx_spline_base;

typedef struct
{
    _dedx_spline_base base[_DEDX_MAXELEMENTS];
    int n;
    int prog;
    int target;
    int ion;
    int datapoints;
    _dedx_lookup_accelerator acc;
} _dedx_lookup_data;


typedef struct
{
    _dedx_lookup_data ** loaded_data;
    int datasets;
    int active_datasets;
} dedx_workspace;


int dedx_ws_load_compound_weigth(dedx_workspace * ws, int prog, int ion, int * targets, float * weight, int length, int * err);
int dedx_ws_load_config(dedx_workspace * ws, int program, int ion, int target, int * use_bragg, int * err);
float dedx_ws_get_stp(dedx_workspace * ws, int id, float energy, int * err);
int dedx_ws_load_compound(dedx_workspace * ws, int prog, int ion, int * targets, int * compos, int length, int * err);
int dedx_ws_load_bethe_config(dedx_workspace * ws, int ion, int target, float pot, int * err);
dedx_workspace * dedx_allocate_workspace(int count, int *err);
void dedx_deallocate_workspace(dedx_workspace * workspace, int *err);



#endif // DEDX_WS
