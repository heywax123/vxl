#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>


#include <vil/vil_convert.h>
#include <vil/vil_image_view.h>

#include <vil/vil_crop.h>
#include <vil/vil_save.h>
#include <vil/vil_rotate.h>
#include <vnl/vnl_math.h>

#include "brad_sam_template_match.h"
#include "brad_multispectral_functions.h"


//------------------------------------------------------------
bool brad_sam_template_match(
  const vil_image_view<float>& img,
  const vil_image_view<float>& chip,
  const vil_image_view<vxl_byte>& chip_mask,
  vil_image_view<float>& sam,
  float angle_step )
{
  int img_width = img.ni(), img_height = img.nj();
  int num_channels = img.nplanes();

  if (chip.nplanes() != num_channels) return false;

  // Initialize the output image
  sam.set_size(img_width, img_height);
  sam.fill(1.0);

  // Setup a normalized channel-aligned image
  vil_image_view<float> norm_img(img_width, img_height, 1, num_channels);
  for (int y = 0; y < img_height; y++) {
    for (int x = 0; x < img_height; x++) {
      for (int c = 0; c < num_channels; c++)
        norm_img(x, y, c) = img(x, y, c);
      brad_normalize_spectra(&norm_img(x, y), num_channels);
    }
  }

  vil_image_view<float> rot_chip, rot_mask;

  // Iterate over angles
  for (float a = 0.0f; a < 360.0f; a += angle_step) {

    // Rotate the chip and mask
    if (a == 0.0f) {
      rot_chip.deep_copy(chip);
      rot_mask.deep_copy(chip_mask);
    }
    else {
      vil_rotate_image(chip, rot_chip, a);
      vil_rotate_image(chip_mask, rot_mask, a);
    }

    int chip_width = rot_chip.ni(), chip_height = rot_chip.nj();
    int off_x = chip_width / 2, off_y = chip_height / 2;

    std::cerr << rot_chip.ni() << ' ' << rot_chip.nj() << '\n';

    // Setup a normalized channel-aligned chip
    vil_image_view<float> norm_chip(chip_width, chip_height, 1, num_channels);
    for (int y = 0; y < chip_height; y++) {
      for (int x = 0; x < chip_height; x++) {
        for (int c = 0; c < num_channels; c++)
          norm_chip(x, y, c) = rot_chip(x, y, c);
        brad_normalize_spectra(&norm_chip(x, y), num_channels);
      }
    }

    // Iterate over the image
    for (int y = 0; y < img_height - chip_height; y++) {
      for (int x = 0; x < img_width - chip_width; x++) {
        for (int dy = 0; dy < chip_height; dy++) {
          for (int dx = 0; dx < chip_width; dx++) {

            // Skip if not in mask
            if (rot_mask(dx, dy) == (vxl_byte)0) continue;

            // Compute spectral angle and take min
            float s = brad_compute_spectral_angle(
              &norm_chip(x, y), &norm_img(x + dx, y + dy), num_channels);
            sam(x + off_x, y + off_y) = std::min(s, sam(x + off_x, y + off_y));
          } //dx
        } //dy
      } //x
    } //y

  } // a

  return true;
}