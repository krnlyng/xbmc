#pragma once

#include <array>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <wayland-client.hpp>

struct qt_surface_extension;
struct qt_extended_surface;

namespace wayland
{
class qt_surface_extension_t;
class qt_extended_surface_t;
enum class qt_extended_surface_orientation : uint32_t;
enum class qt_extended_surface_windowflag : uint32_t;

namespace detail
{
  extern const wl_interface qt_surface_extension_interface;
  extern const wl_interface qt_extended_surface_interface;
}

/** \brief 

*/
class qt_surface_extension_t : public proxy_t
{
private:
  struct events_t : public detail::events_base_t
  {
  };

  static int dispatcher(uint32_t opcode, std::vector<detail::any> args, std::shared_ptr<detail::events_base_t> e);

  qt_surface_extension_t(proxy_t const &wrapped_proxy, construct_proxy_wrapper_tag);

public:
  qt_surface_extension_t();
  explicit qt_surface_extension_t(const proxy_t &proxy);
  qt_surface_extension_t(qt_surface_extension *p, wrapper_type t = wrapper_type::standard);

  qt_surface_extension_t proxy_create_wrapper();

  static const std::string interface_name;

  operator qt_surface_extension*() const;

  /** \brief 
      \param surface 

  */
  qt_extended_surface_t get_extended_surface(surface_t surface);

  /** \brief Minimum protocol version required for the \ref get_extended_surface function
  */
  static constexpr std::uint32_t get_extended_surface_since_version = 1;

};


/** \brief 

*/
class qt_extended_surface_t : public proxy_t
{
private:
  struct events_t : public detail::events_base_t
  {
    std::function<void(int32_t)> onscreen_visibility;
    std::function<void(std::string, array_t)> set_generic_property;
    std::function<void()> close;
  };

  static int dispatcher(uint32_t opcode, std::vector<detail::any> args, std::shared_ptr<detail::events_base_t> e);

  qt_extended_surface_t(proxy_t const &wrapped_proxy, construct_proxy_wrapper_tag);

public:
  qt_extended_surface_t();
  explicit qt_extended_surface_t(const proxy_t &proxy);
  qt_extended_surface_t(qt_extended_surface *p, wrapper_type t = wrapper_type::standard);

  qt_extended_surface_t proxy_create_wrapper();

  static const std::string interface_name;

  operator qt_extended_surface*() const;

  /** \brief 
      \param name 
      \param value 

  */
  void update_generic_property(std::string name, array_t value);

  /** \brief Minimum protocol version required for the \ref update_generic_property function
  */
  static constexpr std::uint32_t update_generic_property_since_version = 1;

  /** \brief 
      \param orientation 

  */
  void set_content_orientation(int32_t orientation);

  /** \brief Minimum protocol version required for the \ref set_content_orientation function
  */
  static constexpr std::uint32_t set_content_orientation_since_version = 1;

  /** \brief 
      \param flags 

  */
  void set_window_flags(int32_t flags);

  /** \brief Minimum protocol version required for the \ref set_window_flags function
  */
  static constexpr std::uint32_t set_window_flags_since_version = 1;

  /** \brief 

  */
  void raise();

  /** \brief Minimum protocol version required for the \ref raise function
  */
  static constexpr std::uint32_t raise_since_version = 1;

  /** \brief 

  */
  void lower();

  /** \brief Minimum protocol version required for the \ref lower function
  */
  static constexpr std::uint32_t lower_since_version = 1;

  /** \brief 
      \param orientation 

  */
  void set_content_orientation_mask(int32_t orientation);

  /** \brief Minimum protocol version required for the \ref set_content_orientation_mask function
  */
  static constexpr std::uint32_t set_content_orientation_mask_since_version = 2;

  /** \brief Check whether the \ref set_content_orientation_mask function is available with
      the currently bound version of the protocol
  */
  bool can_set_content_orientation_mask() const;

  /** \brief 
      \param visible 

  */
  std::function<void(int32_t)> &on_onscreen_visibility();

  /** \brief 
      \param name 
      \param value 

  */
  std::function<void(std::string, array_t)> &on_set_generic_property();

  /** \brief 

  */
  std::function<void()> &on_close();

};

/** \brief 

  */
enum class qt_extended_surface_orientation : uint32_t
  {
  PrimaryOrientation = 0,
  PortraitOrientation = 1,
  LandscapeOrientation = 2,
  InvertedPortraitOrientation = 4,
  InvertedLandscapeOrientation = 8
};

/** \brief 

  */
enum class qt_extended_surface_windowflag : uint32_t
  {
  OverridesSystemGestures = 1,
  StaysOnTop = 2,
  BypassWindowManager = 4
};



}
