#include "../vowpalwabbit/example_predict.h"
#include "../vowpalwabbit/array_parameters.h"
#include "../vowpalwabbit/slim/include/example_predict_builder.h"
#include "../vowpalwabbit/slim/include/vw_slim_predict.h"

#include <emscripten/bind.h>
#include <stdlib.h>

using namespace emscripten;

example_predict* new_example_predict()
{
  return new example_predict();
}

void destroy_example_predict(example_predict* ex)
{
  delete ex;
}

void push_feature_string(vw_slim::example_predict_builder& arr, const std::string& feature_idx, float value) {
    arr.push_feature_string((char*)feature_idx.c_str(), value);
}

int load_model(vw_slim::vw_predict<dense_parameters>& vw, size_t _bytes, int size) {
  char *bytes = (char*)_bytes;
  int res = vw.load(bytes, size);
  free(bytes);
  return res;
}

struct predict_results
{
  std::vector<float> pdf;
  std::vector<int> ranking;

  std::vector<float> get_pdf()
  {
    return pdf;
  }

  std::vector<int> get_ranking()
  {
    return ranking;
  }
};

struct action_list
{
  std::vector<example_predict*> actions;
  void add_action(example_predict* a)
  {
    actions.push_back(a);
  }
};

predict_results predict(vw_slim::vw_predict<dense_parameters>& vw, const std::string& event_id, example_predict* shared, const action_list& actions)
{
  predict_results results;
  printf("event %s shared %p actions %lu\n", event_id.c_str(), shared, actions.actions.size());
  int res = vw.predict(event_id.c_str(), *shared, (example_predict*)actions.actions.data(), actions.actions.size(), results.pdf, results.ranking);
  printf("got %lu pdf and %lu ranking => %d\n", results.pdf.size(), results.ranking.size(), res);
  printf("r: %d %d %d %d %d\n", results.ranking[0], results.ranking[1], results.ranking[2], results.ranking[3], results.ranking[4]);
  return results;
}


EMSCRIPTEN_BINDINGS(vw) {

  class_<example_predict>("example_predict");

  function("new_example_predict", &new_example_predict, allow_raw_pointers());
  function("destroy_example_predict", &destroy_example_predict, allow_raw_pointers());

  class_<vw_slim::example_predict_builder>("example_predict_builder")
    .constructor<example_predict*, const std::string&>()
    .function("push_feature_string", &push_feature_string, allow_raw_pointers())
    .function("push_feature", &vw_slim::example_predict_builder::push_feature, allow_raw_pointers());

  class_<vw_slim::vw_predict<dense_parameters>>("vw_predict")
    .constructor<>()
    .function("load_model", &load_model, allow_raw_pointers())
    .function("predict", &predict, allow_raw_pointers());

  class_<predict_results>("predict_results")
    .function("get_pdf", &predict_results::get_pdf)
    .function("get_ranking", &predict_results::get_ranking);

  class_<action_list>("action_list")
    .constructor<>()
    .function("add_action", &action_list::add_action, allow_raw_pointers());

  register_vector<int>("VectorInt");
  register_vector<float>("VectorFloat");
};