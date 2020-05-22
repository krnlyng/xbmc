#include "qt_surface_extension.hpp"

using namespace wayland;
using namespace detail;

const wl_interface* qt_surface_extension_interface_get_extended_surface_request[2] = {
  &qt_extended_surface_interface,
  &surface_interface,
};

const wl_message qt_surface_extension_interface_requests[1] = {
  {
    "get_extended_surface",
    "no",
    qt_surface_extension_interface_get_extended_surface_request,
  },
};

const wl_message qt_surface_extension_interface_events[0] = {
};

const wl_interface wayland::detail::qt_surface_extension_interface =
  {
    "qt_surface_extension",
    2,
    1,
    qt_surface_extension_interface_requests,
    0,
    qt_surface_extension_interface_events,
  };

const wl_interface* qt_extended_surface_interface_update_generic_property_request[2] = {
  NULL,
  NULL,
};

const wl_interface* qt_extended_surface_interface_set_content_orientation_request[1] = {
  NULL,
};

const wl_interface* qt_extended_surface_interface_set_window_flags_request[1] = {
  NULL,
};

const wl_interface* qt_extended_surface_interface_raise_request[0] = {
};

const wl_interface* qt_extended_surface_interface_lower_request[0] = {
};

const wl_interface* qt_extended_surface_interface_set_content_orientation_mask_request[1] = {
  NULL,
};

const wl_interface* qt_extended_surface_interface_onscreen_visibility_event[1] = {
  NULL,
};

const wl_interface* qt_extended_surface_interface_set_generic_property_event[2] = {
  NULL,
  NULL,
};

const wl_interface* qt_extended_surface_interface_close_event[0] = {
};

const wl_message qt_extended_surface_interface_requests[6] = {
  {
    "update_generic_property",
    "sa",
    qt_extended_surface_interface_update_generic_property_request,
  },
  {
    "set_content_orientation",
    "i",
    qt_extended_surface_interface_set_content_orientation_request,
  },
  {
    "set_window_flags",
    "i",
    qt_extended_surface_interface_set_window_flags_request,
  },
  {
    "raise",
    "",
    qt_extended_surface_interface_raise_request,
  },
  {
    "lower",
    "",
    qt_extended_surface_interface_lower_request,
  },
  {
    "set_content_orientation_mask",
    "2i",
    qt_extended_surface_interface_set_content_orientation_mask_request,
  },
};

const wl_message qt_extended_surface_interface_events[3] = {
  {
    "onscreen_visibility",
    "i",
    qt_extended_surface_interface_onscreen_visibility_event,
  },
  {
    "set_generic_property",
    "sa",
    qt_extended_surface_interface_set_generic_property_event,
  },
  {
    "close",
    "",
    qt_extended_surface_interface_close_event,
  },
};

const wl_interface wayland::detail::qt_extended_surface_interface =
  {
    "qt_extended_surface",
    2,
    6,
    qt_extended_surface_interface_requests,
    3,
    qt_extended_surface_interface_events,
  };

qt_surface_extension_t::qt_surface_extension_t(const proxy_t &p)
  : proxy_t(p)
{
  if(proxy_has_object() && get_wrapper_type() == wrapper_type::standard)
    {
      set_events(std::shared_ptr<detail::events_base_t>(new events_t), dispatcher);
    }
  interface = &qt_surface_extension_interface;
  copy_constructor = [] (const proxy_t &p) -> proxy_t
    { return qt_surface_extension_t(p); };
}

qt_surface_extension_t::qt_surface_extension_t()
{
  interface = &qt_surface_extension_interface;
  copy_constructor = [] (const proxy_t &p) -> proxy_t
    { return qt_surface_extension_t(p); };
}

qt_surface_extension_t::qt_surface_extension_t(qt_surface_extension *p, wrapper_type t)
  : proxy_t(reinterpret_cast<wl_proxy*> (p), t){
  if(proxy_has_object() && get_wrapper_type() == wrapper_type::standard)
    {
      set_events(std::shared_ptr<detail::events_base_t>(new events_t), dispatcher);
    }
  interface = &qt_surface_extension_interface;
  copy_constructor = [] (const proxy_t &p) -> proxy_t
    { return qt_surface_extension_t(p); };
}

qt_surface_extension_t::qt_surface_extension_t(proxy_t const &wrapped_proxy, construct_proxy_wrapper_tag)
  : proxy_t(wrapped_proxy, construct_proxy_wrapper_tag()){
  interface = &qt_surface_extension_interface;
  copy_constructor = [] (const proxy_t &p) -> proxy_t
    { return qt_surface_extension_t(p); };
}

qt_surface_extension_t qt_surface_extension_t::proxy_create_wrapper()
{
  return {*this, construct_proxy_wrapper_tag()};
}

const std::string qt_surface_extension_t::interface_name = "qt_surface_extension";

qt_surface_extension_t::operator qt_surface_extension*() const
{
  return reinterpret_cast<qt_surface_extension*> (c_ptr());
}

qt_extended_surface_t qt_surface_extension_t::get_extended_surface(surface_t surface)
{
  proxy_t p = marshal_constructor(0u, &qt_extended_surface_interface, nullptr, surface.proxy_has_object() ? reinterpret_cast<wl_object*>(surface.c_ptr()) : nullptr);
  return qt_extended_surface_t(p);
}

int qt_surface_extension_t::dispatcher(uint32_t opcode, std::vector<any> args, std::shared_ptr<detail::events_base_t> e)
{
  return 0;
}

qt_extended_surface_t::qt_extended_surface_t(const proxy_t &p)
  : proxy_t(p)
{
  if(proxy_has_object() && get_wrapper_type() == wrapper_type::standard)
    {
      set_events(std::shared_ptr<detail::events_base_t>(new events_t), dispatcher);
    }
  interface = &qt_extended_surface_interface;
  copy_constructor = [] (const proxy_t &p) -> proxy_t
    { return qt_extended_surface_t(p); };
}

qt_extended_surface_t::qt_extended_surface_t()
{
  interface = &qt_extended_surface_interface;
  copy_constructor = [] (const proxy_t &p) -> proxy_t
    { return qt_extended_surface_t(p); };
}

qt_extended_surface_t::qt_extended_surface_t(qt_extended_surface *p, wrapper_type t)
  : proxy_t(reinterpret_cast<wl_proxy*> (p), t){
  if(proxy_has_object() && get_wrapper_type() == wrapper_type::standard)
    {
      set_events(std::shared_ptr<detail::events_base_t>(new events_t), dispatcher);
    }
  interface = &qt_extended_surface_interface;
  copy_constructor = [] (const proxy_t &p) -> proxy_t
    { return qt_extended_surface_t(p); };
}

qt_extended_surface_t::qt_extended_surface_t(proxy_t const &wrapped_proxy, construct_proxy_wrapper_tag)
  : proxy_t(wrapped_proxy, construct_proxy_wrapper_tag()){
  interface = &qt_extended_surface_interface;
  copy_constructor = [] (const proxy_t &p) -> proxy_t
    { return qt_extended_surface_t(p); };
}

qt_extended_surface_t qt_extended_surface_t::proxy_create_wrapper()
{
  return {*this, construct_proxy_wrapper_tag()};
}

const std::string qt_extended_surface_t::interface_name = "qt_extended_surface";

qt_extended_surface_t::operator qt_extended_surface*() const
{
  return reinterpret_cast<qt_extended_surface*> (c_ptr());
}

void qt_extended_surface_t::update_generic_property(std::string name, array_t value)
{
  marshal(0u, name, value);
}

void qt_extended_surface_t::set_content_orientation(int32_t orientation)
{
  marshal(1u, orientation);
}

void qt_extended_surface_t::set_window_flags(int32_t flags)
{
  marshal(2u, flags);
}

void qt_extended_surface_t::raise()
{
  marshal(3u);
}

void qt_extended_surface_t::lower()
{
  marshal(4u);
}

void qt_extended_surface_t::set_content_orientation_mask(int32_t orientation)
{
  marshal(5u, orientation);
}
bool qt_extended_surface_t::can_set_content_orientation_mask() const
{
  return (get_version() >= set_content_orientation_mask_since_version);
}

std::function<void(int32_t)> &qt_extended_surface_t::on_onscreen_visibility()
{
  return std::static_pointer_cast<events_t>(get_events())->onscreen_visibility;
}

std::function<void(std::string, array_t)> &qt_extended_surface_t::on_set_generic_property()
{
  return std::static_pointer_cast<events_t>(get_events())->set_generic_property;
}

std::function<void()> &qt_extended_surface_t::on_close()
{
  return std::static_pointer_cast<events_t>(get_events())->close;
}

int qt_extended_surface_t::dispatcher(uint32_t opcode, std::vector<any> args, std::shared_ptr<detail::events_base_t> e)
{
  std::shared_ptr<events_t> events = std::static_pointer_cast<events_t>(e);
  switch(opcode)
    {
    case 0:
      if(events->onscreen_visibility) events->onscreen_visibility(args[0].get<int32_t>());
      break;
    case 1:
      if(events->set_generic_property) events->set_generic_property(args[0].get<std::string>(), args[1].get<array_t>());
      break;
    case 2:
      if(events->close) events->close();
      break;
    }
  return 0;
}




