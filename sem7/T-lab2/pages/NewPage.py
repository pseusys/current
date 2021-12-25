from selenium.common.exceptions import TimeoutException
from selenium.webdriver import Chrome

from pages._xpath import Tag, Param, Condition
import pages


class NewPage(pages.BasePage):
    def __init__(self, driver: Chrome, username: str, *args, **kwargs):
        super(NewPage, self).__init__(driver, username, "new", *args, **kwargs)

    def create_repo(self, name: str, desc="", private=False, readme=False) -> pages.RepoPage:
        self._fill_form(("repository[name]", name), ("repository[description]", desc))
        self._scroll_to_bottom()
        if private:
            self._find_visible_enabled(Tag.Input, (Param.Id, Condition.Equals, "repository_visibility_private")).click()
        if readme:
            self._find_visible_enabled(Tag.Input, (Param.Id, Condition.Equals, "repository_auto_init")).click()
        try:
            self._await_element_to_enable(Tag.Button, (Param.Type, Condition.Equals, "submit"), (Param.Class, Condition.Contains, "btn-primary")).click()
            return pages.RepoPage(self._driver, self._username, name)
        except TimeoutException:
            raise RuntimeError(f"Repo create button is disabled! Most likely repo with the name '{name}' already exist!")
