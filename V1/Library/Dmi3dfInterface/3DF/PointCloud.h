#pragma once

#include <HIOManager.h>
#include <stdint.h>
#include "vbsp.h"

class HBaseView;

class  HPointCloudOptions {
public:
    /*!
        Constructs the HPointCloudOptions object and sets the defaults by calling Defaults.
        */
    HPointCloudOptions() { Defaults(); }

    ~HPointCloudOptions() {}

    /*!
        This method sets the default values for the various control points.
        */
    void Defaults()
    {
        m_numBuckets = 512;
        m_lowIntensityValue = 0;
        m_highIntensityValue = 0;
        m_maxShellSize = 10000;
        m_minShellSize = 2000;
    }

    /*!
        This field sets the number of buckets to bin gray scale values into.  If it is set to zero, no down-sampling will occur.
        This field only affects gray scale point clouds.
        */
    unsigned int m_numBuckets;
    /*!
        This field sets the lower limit on the intensity values to be found in the file.  If it is set to zero, this value is
        determined automatically from the data.
        */
    int m_lowIntensityValue;
    /*!
        This field sets the upper limit on the intensity values to be found in the file.  If it is set to zero, this value is
        determined automatically from the data.
    */
    int m_highIntensityValue;
    /*!
        This field sets the maximum number of points to have in a spatially segregated shell.
    */
    int m_maxShellSize;
    /*!
        This field sets the minimum number of points to have in a spatially segregated shell.
    */
    int m_minShellSize;
};

class PCPoint;
class RGBColor;
class BSPData;
typedef DefaultBSPNodeItemContainer<PCPoint *> PCPointContainer;

/*!
    The PointCloud class is used for loading point cloud data into HOOPS.  This class is designed for
    in-core usage, i.e., the point set must be small enough to fit in memory.  The reading happens in three passes:
        - The first pass obtains an accurate bounding box for the point cloud and generates a (temporary) binary version of the file
        to facilitate quicker processing in the subsequent pass.
        - The second pass reads the binary version of the point cloud, and sorts the points in an octree structure.  It generates
    shells of a user-specified size (see the #HPointCloudOptions class) and stores these in a (temporary) HSF-like binary file to
    ultimately be read into the scene graph.
        - The third pass reads the stored shells generated from the previous step and inserts HOOPS shells into the scene graph.

    Note: The temporary files will be removed by the reader when it no longer needs them.  However, the size of these files will
    be relative to the size of the original point cloud - so one will need to have the necessary disk space accomodate these files
    (albeit temporarily) when importing.
    */

enum PointCloudFileType { NONE, UNKNOWN, PTS, PTX, XYZ };

class  PointCloud : public HInputHandler {
public:
    PointCloud();
    ~PointCloud();

    /*!
        This method loads a point cloud to a given HOOPS segment key.
            \param filename The name of the file to load.
            \param key The HOOPS segment key to load model data into.
            \param options An options class containing any needed extra information for the operation.
            \return The #HFileInputResult result of the input operation.
        */
    HFileInputResult FileInputByKey(__wchar_t const * filename, HC_KEY key, HInputHandlerOptions * options);

    HFileInputResult FileInputByKey(unsigned short const * filename, HC_KEY key, HInputHandlerOptions * options)
    {
        H_UTF16 utf16;
        utf16.encodedText(filename);
        return FileInputByKey(H_WCS(utf16).encodedText(), key, options);
    };

    HFileInputResult FileInputByKey(char const * filename, HC_KEY key, HInputHandlerOptions * options)
    {
        return FileInputByKey(H_WCS(filename).encodedText(), key, options);
    };

    /*! \return A character pointer denoting the name of the handler.
        */
    char const * GetInputName() { return "PointCloud"; }

    /*!
        This method returns the extensions handled by the reader.
        */
    char const * GetInputTypesString();

    HInputHandlerStyle GetInputStyle() { return HInputHandlerStyleModel; };

    /*!
        This method registers the "pts" and "ptx" extensions as those handled by the reader.
        */
    void RegisterInputHandlerTypes();

private:
    bool Initialize(__wchar_t const * filename);
    HFileInputResult SpatiallySortPointCloud(HC_KEY seg_key);

    float GetScaledIntensity(PCPoint const * cpt);

    // functions when walking the hash
    static void DeleteBSPs(VBSP<PCPoint *> * bsp, int bucket, void const * const user_data);
    static void WriteShellsToFile(VBSP<PCPoint *> * bsp, int bucket, void const * const user_data);

    // function when walking the BSP
    static VBSPStatus GetShellsFromBSP(PCPointContainer * container, void * user_data);

    bool InsertShells(BSPData * bsp_data);

    FILE * m_file;
    int64_t m_file_size;
    wchar_t m_bin_file_name[MVO_BUFFER_SIZE];
    FILE * m_bin_file;

    PointCloudFileType m_point_cloud_file_type;
    unsigned long m_num_points;
    double m_std_bbox[6];
    float m_matrix[16];
    int m_min_intensity;
    int m_max_intensity;

    HBaseView * m_pHView;
    HPointCloudOptions * m_point_cloud_options;
};
