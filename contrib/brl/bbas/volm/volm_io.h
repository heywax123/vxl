//This is brl/bbas/volm/volm_io.h
#ifndef volm_io_h_
#define volm_io_h_
//:
// \file
// \brief A class to hold xml file io methods for volumetric matchers
// Units are in meters
//
// \author Ozge C. Ozcanli
// \date September 18, 2012
// \verbatim
//  Modifications
//   Yi Dong - October 25 2012 - modify the read_labelme to construct depth_map_scene from xml file
// \endverbatim

#include <vcl_string.h>
#include <bpgl/depth_map/depth_map_scene_sptr.h>
#include <bpgl/depth_map/depth_map_scene.h>
#include <vcl_set.h>
#include <vcl_utility.h>

class volm_orient_table
{
 public:
  static vcl_map<vcl_string, depth_map_region::orientation> ori_id;

  // list of the possible values for indexed orientations from the reference world
  static vcl_map<int, vil_rgb<vxl_byte> >  ori_index_colors;
};


class volm_nlcd_table
{
 public:
  enum nlcd_values {SAND = 31, WATER = 11, DEVELOPED_LOW = 22, DEVELOPED_MED = 23, DEVELOPED_HIGH = 24, DEVELOPED_OPEN = 21, WETLAND = 95, WOODY_WETLAND = 90, DOCK = 100, BUILDING = 101};
  static vcl_map<int, vcl_pair<unsigned char, vil_rgb<vxl_byte> > > land_id ;
};

class volm_io_expt_params
{
 public:
  float fov_inc, tilt_inc, roll_inc, head_inc, vmin, solid_angle, dmax, cap_angle, point_angle, top_angle, bottom_angle;
  void read_params(vcl_string params_file);
};

class volm_io
{
 public:

  //: warning: always add to the end of this error code list, python script on the server has a hard copy of some of these values, they should not be changed
  enum VOLM_ERROR_CODES {SUCCESS, EXE_ARGUMENT_ERROR, EXE_RUNNING, CAM_FILE_IO_ERROR, MATCHER_EXE_STARTED, MATCHER_EXE_FINISHED, MATCHER_EXE_FAILED, COMPOSE_STARTED, LABELME_FILE_IO_ERROR, GEO_INDEX_FILE_MISSING, SCORE_FILE_MISSING, EXE_STARTED, EXE_MATCHER_FAILED, COMPOSE_HALT};

  //: scale value is STRONG_POSITIVE-STRONG_NEGATIVE
  enum VOLM_IMAGE_CODES {UNEVALUATED = 0, STRONG_NEGATIVE = 1, UNKNOWN = 127, STRONG_POSITIVE = 255, SCALE_VALUE = 254};

  static bool write_status(vcl_string out_folder, int status_code, int percent=0, vcl_string log_message = "");
  static bool write_log(vcl_string out_folder, vcl_string log);
  static bool write_composer_log(vcl_string out_folder, vcl_string log);

  //: return true if MATCHER_EXE_FINISHED, otherwise return false
  static bool check_matcher_status(vcl_string out_folder);

  static bool read_camera(vcl_string kml_file,
                          unsigned const& ni, unsigned const& nj,
                          double& heading,   double& heading_dev,
                          double& tilt,      double& tilt_dev,
                          double& roll,      double& roll_dev,
                          double& top_fov,   double& top_fov_dev,
                          double& altitude, double& lat, double& lon);

  static bool read_labelme(vcl_string xml_file, depth_map_scene_sptr& depth_scene, vcl_string& img_category);

  //: piecewise linear s.t. [1,127) -> [0,t), [127,255] -> [t,1]
  static float scale_score_to_0_1(unsigned char pix_value, float threshold);
  //: piecewise linear s.t. [0,t) -> [1,127), [t,1] -> [127,255]"
  static unsigned char scale_score_to_1_255(float threshold, float score);
  //: piecewise linear s.t. [0,t) -> [1,63), [t,1] -> [63,127]"
  static unsigned char scale_score_to_1_127(float threshold, float score);

  //: read the specific polygon format given by python parser for candidate list processing
  static void read_polygons(vcl_string poly_file, vgl_polygon<double>& out);
  static void convert_polygons(vgl_polygon<double> const& in, vgl_polygon<float>& out);

  static int read_gt_file(vcl_string gt_file, vcl_vector<vcl_pair<vgl_point_3d<double>, vcl_pair<vcl_string, vcl_string> > >& samples);
  static bool read_ray_index_data(vcl_string path, vcl_vector<unsigned char>& data);

  //: read the building footpring file
  static bool read_building_file(vcl_string file, vcl_vector<vgl_polygon<double> >& builds);
};

class volm_rationale;
bool operator>(const vcl_pair<float, volm_rationale>& a, const vcl_pair<float, volm_rationale>& b);

class volm_rationale
{
 public:
  float lat;
  float lon;
  float elev;
  unsigned index_id; // also index in score.bin and cam.bin files
  unsigned cam_id;   // index for camera in volm_query
  vcl_string index_file;
  vcl_string score_file;

  static bool write_top_matches(vcl_multiset<vcl_pair<float, volm_rationale>, std::greater<vcl_pair<float, volm_rationale> > >& top_matches, vcl_string& filename);
  static bool read_top_matches(vcl_multiset<vcl_pair<float, volm_rationale>, std::greater<vcl_pair<float, volm_rationale> > >& top_matches, vcl_string& filename);
};

// \brief  A class to store the highest score for each location
// leaf_id     ----> id of the leaf in leaves vector
// hypo_id     ----> local id of the hypothesis in the leaf
// max_score_  ----> highest score for current location
// max_cam_id_ ----> the camera id associated with the highest score
// cam_id      ----> vector of camera ids whose score is higher than defined threshold

#include <vbl/vbl_smart_ptr.h>
class volm_score;
typedef vbl_smart_ptr<volm_score> volm_score_sptr;

class volm_score : public vbl_ref_count
{
 public:
  volm_score () {}
  volm_score(unsigned leaf_id, unsigned hypo_id) : leaf_id_(leaf_id), hypo_id_(hypo_id) {}
  volm_score(unsigned const& leaf_id, unsigned const& hypo_id, float const& max_score, unsigned const& max_cam_id, vcl_vector<unsigned> const& cam_id)
    : leaf_id_(leaf_id), hypo_id_(hypo_id), max_score_(max_score), max_cam_id_(max_cam_id), cam_id_(cam_id) {}
  ~volm_score() {}
  unsigned leaf_id_;
  unsigned hypo_id_;
  float    max_score_;
  unsigned max_cam_id_;
  vcl_vector<unsigned> cam_id_;

  //: version
  unsigned version() const {return 1;}

  //: binary IO write
  void b_write(vsl_b_ostream& os);

  //: binary IO read
  void b_read(vsl_b_istream& is);

  static void write_scores(vcl_vector<volm_score_sptr>& scores, vcl_string const& file_name);
  static void read_scores(vcl_vector<volm_score_sptr>& scores, vcl_string const& file_name);
};


#endif // volm_io_h_
